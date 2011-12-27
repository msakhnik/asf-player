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
    ~cAsfPlayer();

    bool Play();

    void SetFullScreen(bool flag) { _full_screen = flag; }
    void SetFrameByFrame(bool flag) { _frame_by_frame = flag; }

    bool GetFullScreen() const { return _full_screen; }
    bool GetFrameByFrame() const { return _frame_by_frame; }

    bool Init();

private:
    cAsfFile &_file;
    bool _full_screen;
    bool _frame_by_frame;

    IplImage* _img;
    uchar* _data;

    bool _ShowFrame();

    // Wait for user input atmost @msec milliseconds
    int _WaitForKey(int msec = 0);
};

// vim: set et ts=4 sw=4:
