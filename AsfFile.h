//
// File:   AsfFile.h
// Author: morwin
//
// Created on 26 листопада 2011, 17:35
//

#pragma once

#include <string>
#include <map>
#include <vector>
#include <fstream>

class cAsfFile
{
public:

    cAsfFile(const std::string&);


    // read from file
    bool ReadHeader();
    std::vector<int> ReadFrame();

    typedef std::map<std::string, std::string> MapType;

    MapType info;

    //basic parameters
    std::string data_type;
    std::string version;
    double seconds_per_frame;
    unsigned int rows;
    unsigned int cols;
    int noise_threshold;
    unsigned int start_frame;
    unsigned int end_frame;
    std::string ascii_data;

private:
    std::string _filename;
    std::ifstream _file;
};
