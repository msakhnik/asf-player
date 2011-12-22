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

class cAsfFile;

class cAsfPlayer
{
public:

    cAsfPlayer(cAsfFile &);

    bool Play();

private:
    cAsfFile &_file;

    IplImage* img;
    uchar* data;

    bool _ShowFrame();
};
