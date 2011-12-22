//
// File:   AsfPlayer.h
// Author: morwin
//
// Created on 23 листопада 2011, 11:47
//

#pragma once

#include "AsfFile.h"

//openCv
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>

class cAsfPlayer
{
public:

    cAsfPlayer(const cAsfFile &);

    bool Play();

private:

    bool _ShowFrame();
    IplImage* img;
    uchar* data;

    cAsfFile _file;
};
