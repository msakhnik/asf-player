//
// File:   AsfFile.h
// Author: morwin
//
// Created on 26 листопада 2011, 17:35
//

#pragma once

#include "AsfPlayer.h"

#include <string>
#include <map>

class AsfFile
{
public:

    AsfFile(const std::string&);

    //print list headers
    void Get_Header();

    // read from file
    bool Read_File();

    void Set_Full_Screen();
    void Set_Frame_By_Frame();

    bool Write_File();

    //header іnformation
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

    void Read_Line(std::string &, std::vector<std::string> &, char);

    //max image
    std::vector<int> data_image;

    std::string filename;
    AsfPlayer player;
};
