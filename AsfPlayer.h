//
// File:   AsfPlayer.h
// Author: morwin
//
// Created on 23 листопада 2011, 11:47
//

#pragma once

#include <vector>

//openCv
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

class AsfPlayer {
public:

    AsfPlayer();

    bool Show_Image(unsigned, const std::vector<int> &);

    int Record_Video(std::vector<int> &);
    
    bool full_screen;
    bool frame_by_frame;

    double seconds_per_frame;
    unsigned int rows;
    unsigned int cols;
    unsigned int start_frame;
    unsigned int end_frame;

};
