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

    void SetFullScreen(bool flag) {_full_screen = flag; };
    void SetFrameByFrame(bool flag) {_frame_by_frame = flag; };

    bool GetFullScreen() {return _full_screen; };
    bool GetFrameByFrame() {return _frame_by_frame;};

    cAsfPlayer(cAsfFile &);
    ~cAsfPlayer();

    bool Play();

private:
    cAsfFile &_file;

    IplImage* img;
    uchar* data;

    bool _full_screen;
    bool _frame_by_frame;

    bool _ShowFrame();
};
