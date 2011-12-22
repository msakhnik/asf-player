#include "AsfFile.h"

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
    : _file (file.c_str())
{
    //initial header array
    info["HARDWARE"] = "0";
    info["HW_TYPE"] = "MOVIE";
    info["SENSOR_TYPE"] = "0";
    info["ROW_SPACING"] = "0";
    info["COL_SPACING"] = "0";
    info["SENSEL_AREA"] = "0";
    info["MICRO_SECOND"] = "0";
    info["TIME"] = "0";
    info["SENSITIVITY"] = "0";
    info["UNITS"] = "0";
    info["MIRROR_ROW"] = "0";
    info["MIRROR_COL"] = "0";
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
        value = line.substr(key.length() + 1, line.length() - key.length());

        if (key == "DATA_TYPE")
            this->data_type = value;
        else if (key == "VERSION")
            this->version = value;
        else if (key == "NOISE_THRESHOLD")
            this->noise_threshold = atoi(value.c_str());
        else if (key == "COLS")
            this->cols = atoi(value.c_str());
        else if (key == "ROWS")
            this->rows = atoi(value.c_str());
        else if (key == "START_FRAME")
            this->start_frame = atoi(value.c_str());
        else if (key == "END_FRAME")
            this->end_frame = atoi(value.c_str());
        else if (key == "SECONDS_PER_FRAME")
            this->seconds_per_frame = 1000 * atof(value.c_str());
        else if (key == "ASCII_DATA")
            this->ascii_data = value;
        else
            this->info[key] = value;
    }

    return true;
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
