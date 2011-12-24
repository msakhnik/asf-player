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
    SetInfo("HARDWARE", "0");
    SetInfo("HW_TYPE", "MOVIE");
    SetInfo("SENSOR_TYPE", "0");
    SetInfo("ROW_SPACING", "0");
    SetInfo("COL_SPACING", "0");
    SetInfo("SENSEL_AREA", "0");
    SetInfo("MICRO_SECOND", "0");
    SetInfo("TIME", "0");
    SetInfo("SENSITIVITY", "0");
    SetInfo("UNITS", "0");
    SetInfo("MIRROR_ROW", "0");
    SetInfo("MIRROR_COL", "0");
}

void cAsfFile::ReadHeader()
{
    if (!_file.is_open())
    {
        cerr << "Error reading from file" << endl;
        exit(1);
    }
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
            this->SetDataType(value);
        else if (key == "VERSION")
            this->SetVersion(value);
        else if (key == "NOISE_THRESHOLD")
            this->SetNoiseThreshold(atoi(value.c_str()));
        else if (key == "COLS")
            this->SetCols(atoi(value.c_str()));
        else if (key == "ROWS")
            this->SetRows(atoi(value.c_str()));
        else if (key == "START_FRAME")
            this->SetStartFrame(atoi(value.c_str()));
        else if (key == "END_FRAME")
            this->SetEndFrame(atoi(value.c_str()));
        else if (key == "SECONDS_PER_FRAME")
            this->SetSecondsPerFrame(1000 *
                                     atof(value.c_str()));
        else if (key == "ASCII_DATA")
            this->SetAsciiData(value);
        else
            SetInfo(key, value);
    }
}

vector<int> cAsfFile::ReadFrame()
{

    string line;
    vector<int> data_array;

    while (_file.good())
    {
        getline(_file, line);

        if (line.length() <= 3)
        {
            if (!data_array.empty())
                break;
        }
        else
        {
            //Зчитую рядок
            istringstream is(line);
            string s;

            while (getline(is, s, ','))
                data_array.push_back(atoi(s.c_str()));

        }

    }

    return data_array;
}