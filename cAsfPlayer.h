//
// File:   AsfPlayer.h
// Author: morwin
//
// Created on 23 листопада 2011, 11:47
//

#pragma once

//openCv
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

#include "cAsfFile.h"

class cAsfPlayer
{
public:

    cAsfPlayer(const cAsfFile &);

    bool Play();

private:

    bool ShowFrame();
    IplImage* img;
    uchar* data;

    cAsfFile _file;
};
