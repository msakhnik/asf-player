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

using namespace std;

cAsfFile::cAsfFile(const string& file)
        : _file(file.c_str())
{
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
            this->_data_type = value;
        else if (key == "VERSION")
            this->_version = value;
        else if (key == "NOISE_THRESHOLD")
            this->_noise_threshold = atoi(value.c_str());
        else if (key == "COLS")
            this->_cols = atoi(value.c_str());
        else if (key == "ROWS")
            this->_rows = atoi(value.c_str());
        else if (key == "START_FRAME")
            this->_start_frame = atoi(value.c_str());
        else if (key == "END_FRAME")
            this->_end_frame = atoi(value.c_str());
        else if (key == "SECONDS_PER_FRAME")
            this->_seconds_per_frame = 1000 * atof(value.c_str());
        else if (key == "ASCII_DATA")
            this->_ascii_data = value;
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
    this->_frame_position.resize(this->_end_frame);
    fill (this->_frame_position.begin(), this->_frame_position.end(), 0);

    string line;

    if (!_file.is_open())
        return false;

    while (_file.good())
    {
        getline(_file, line);

        unsigned int row = 0;
        if (line.substr(0, 5) == "Frame")
        {
            //parse string and get number of current frame
            //set in array current position
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
    if (this->_frame_position[pos])
        this->_file.seekg(this->_frame_position[pos]);
}

bool cAsfFile::ReadFrame()
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

    return !_last_frame.empty();
}

// vim: set et ts=4 sw=4:
