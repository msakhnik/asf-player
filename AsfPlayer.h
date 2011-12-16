//
// File:   AsfPlayer.h
// Author: morwin
//
// Created on 23 листопада 2011, 11:47
//

#pragma once

#include "AsfFile.h"

#include <vector>

//openCv
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

class AsfPlayer
{
public:
    AsfPlayer(const std::string&);

    //print list headers
    void get_header();

    // read from file
    bool readFile();

    bool full_screen;
    bool frame_by_frame;

private:
    std::string filename;

    //include asf object
    AsfFile asf_file;

    bool show_image(unsigned);

    void readLine(std::string &, std::vector<std::string> &, char);

    //max image
    vector<int> data_image;

};
