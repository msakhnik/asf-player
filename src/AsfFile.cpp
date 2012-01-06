//
// AsfFile.cpp
//
//     Created: 02.12.2012
//      Author: Sakhnik
//
// This file is part of Asf Player.
//
// Asf Player is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Asf Player License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Asf Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Asf Player.  If not, see <http://www.gnu.org/licenses/>

#include "AsfFile.h"
#include "AsfPlayer.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <ncurses.h>

using namespace std;

cAsfFile::cAsfFile(const string& file)
        : _filename(file.c_str())
{
    _data_type = "MOVIE";
    _version = "webcam 1.0";
    _seconds_per_frame = 0.33;
    _rows = 1;
    _cols = 1;
    _noise_threshold = 0;
    _start_frame = 1;
    _end_frame = 1;
    _ascii_data = "@@";
    //initial header array
    _info["HARDWARE"] = "0";
    _info["HW_TYPE"] = "MOVIE";
    _info["SENSOR_TYPE"] = "0";
    _info["ROW_SPACING"] = "0";
    _info["COL_SPACING"] = "0";
    _info["SENSEL_AREA"] = "0";
    _info["MICRO_SECOND"] = "0";
    _info["TIME"] = "0";
    _info["SENSITIVITY"] = "0";
    _info["UNITS"] = "0";
    _info["MIRROR_ROW"] = "0";
    _info["MIRROR_COL"] = "0";
}

bool cAsfFile::ReadHeader()
{
    _file.open(_filename);
    if (!_file.is_open())
        return false;

    while (_file.good())
    {
        string line;
        getline(_file, line);

        if (line.length() <= 3)
            break;

        istringstream tmp(line);
        string key, value;
        tmp >> key;
        value = line.substr(key.length() + 1,
                            line.length() - key.length());
        if (key == "DATA_TYPE")
            _data_type = value;
        else if (key == "VERSION")
            _version = value;
        else if (key == "NOISE_THRESHOLD")
            _noise_threshold = atoi(value.c_str());
        else if (key == "COLS")
            _cols = atoi(value.c_str());
        else if (key == "ROWS")
            _rows = atoi(value.c_str());
        else if (key == "START_FRAME")
            _start_frame = atoi(value.c_str());
        else if (key == "END_FRAME")
            _end_frame = atoi(value.c_str());
        else if (key == "SECONDS_PER_FRAME")
            _seconds_per_frame = 1000 * atof(value.c_str());
        else if (key == "ASCII_DATA")
            _ascii_data = value;
        else
            _info[key] = value;
    }

    return true;
}
//Static func for parsing input line and returning number of frame
static unsigned int GetNumberFrame(string & str)
{
    string::size_type pos = str.find(",");
    string frame_line = str.substr(0, pos);
    pos = str.find(" ");

    return atoi(str.substr(pos + 1, frame_line.length()).c_str());
}
//Store position all frames
bool cAsfFile::SetPositionFrame()
{
    _frame_position.resize(_end_frame);
    fill (_frame_position.begin(), _frame_position.end(), 0);
    string line;
    if (!_file.is_open())
        return false;
    while (_file.good())
    {
        getline(_file, line);
        unsigned int row = 0;
        if (line.substr(0, 5) == "Frame")
        {
            int skip = _file.tellg();
            row = GetNumberFrame(line);
            _frame_position[row] = skip - line.length() - 1;
            if (row == _end_frame - 1)
            {
                _file.seekg(_frame_position[1]);
                _frame_position[0] = _frame_position[1];
                return true;
            }
        }
    }
    return true;
}
//This func call from trackbar and control key
void cAsfFile::ChangePosition(unsigned int pos)
{
    if (_frame_position[pos])
        _file.seekg(_frame_position[pos]);
}

void cAsfFile::ReadFrame()
{
    _last_frame.clear();
    string line;
    while (_file.good())
    {
        getline(_file, line);
        if (line.length() <= 3 || line.substr(0, 5) == "Frame")
        {
            if (!_last_frame.empty())
                break;
        }
        else
        {
            //Зчитую рядок
            istringstream is(line);
            string s;
            while (getline(is, s, ','))
                _last_frame.push_back(atoi(s.c_str()));
        }
    }

    if (_last_frame.empty())
        cerr << "\nSome data is lost" << endl;;
}

bool cAsfFile::InitRecordFile()
{
    _fileRecord.open("tmp.dat");
    if (!_fileRecord.is_open())
    {
        cerr << "Cannot create file" << endl;
        return false;
    }
    return true;
}

void cAsfFile::RecordFrame(vector<int> & data, unsigned int & frame)
{
    _fileRecord << "\nFrame " << frame << "\n";
    vector<int>::const_iterator iter = data.begin();
    for (unsigned int i = 0; i < _cols; ++i)
    {
        for (unsigned int j = 0; j < _rows; ++j)
        {
            _fileRecord << *(++iter);
            if (j != _rows - 1)
                _fileRecord << ",";
        }
        _fileRecord << "\n";
    }
}
bool cAsfFile::RecordHeader()
{
    ofstream final_file;
    final_file.open(_filename);
    final_file << "DATA_TYPE " << _data_type << "\n";
    final_file << "VERSION " << _version << "\n";
    final_file << "SECONDS_PER_FRAME " << _seconds_per_frame << "\n";
    final_file << "ROWS " << _rows << "\n";
    final_file << "COLS " << _cols << "\n";
    final_file << "NOISE_THRESHOLD " << _noise_threshold << "\n";
    final_file << "START_FRAME " << _start_frame << "\n";
    final_file << "END_FRAME " << _end_frame << "\n";
    final_file << "ASCII_DATA " << _ascii_data << "\n";
    _fileRecord.close();
    ifstream ss;
    char p;
    ss.open("tmp.dat", ios::binary); //отсюда читаем
    ss.seekg(0);
    while (ss)
    {
        ss.read(&p, sizeof (char));
        final_file.write(&p, sizeof (char));
    }
    remove("tmp.dat");
    return true;
}

// vim: set et ts=4 sw=4:
