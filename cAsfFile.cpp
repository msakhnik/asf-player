#include "cAsfFile.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <sstream>
#include <cstdlib>

using namespace std;

cAsfFile::cAsfFile(const string& file)
{

    this->_filename = file;
    this->_position_for_read = 0;

    //initial header array
    this->info["HARDWARE"] = "0";
    this->info["HW_TYPE"] = "MOVIE";
    this->info["SENSOR_TYPE"] = "0";
    this->info["ROW_SPACING"] = "0";
    this->info["COL_SPACING"] = "0";
    this->info["SENSEL_AREA"] = "0";
    this->info["MICRO_SECOND"] = "0";
    this->info["TIME"] = "0";
    this->info["SENSITIVITY"] = "0";
    this->info["UNITS"] = "0";
    this->info["MIRROR_ROW"] = "0";
    this->info["MIRROR_COL"] = "0";
}

bool cAsfFile::ReadHeader()
{

    string line;

    ifstream myfile(this->_filename.c_str());

    if (!myfile.is_open())
        return false;

    while (myfile.good())
    {

        getline(myfile, line);

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

    //Запам"ятовую позицію, для того щоб починати зчитування фреймів не з початку
    this->_position_for_read = myfile.tellg();

    myfile.close();

    return true;
}

bool cAsfFile::ReadFrame()
{

    string line;
    vector<int> data_array;
    unsigned int frame_number = 0;

    ifstream myfile(this->_filename.c_str());

    if (!myfile.is_open())
        return false;

    //Перескакую на позицію першого фрейма
    myfile.seekg(this->_position_for_read);

    while (myfile.good())
    {
        getline(myfile, line);

        if (line.length() <= 3)
        {
            frame_number++;
            if (!data_array.empty())
            {
                cout << "Frame " << frame_number << endl;
               /////////////////////////////////////////////////////?
                copy(data_array.begin(), data_array.end(), ostream_iterator<int>(cout, " "));
                cout << endl;
                /////////////////////////////////////////////////////
                data_array.clear();
            }
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

    myfile.close();

    return true;
}