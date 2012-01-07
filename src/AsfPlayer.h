//
// AsfPlayer.h
//
//     Created: 02.12.2012
//      Author: Sakhnik
//
// This file is part of Asf Player.
//
// Asf Player is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Asf Player License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Asf Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Asf Player.  If not, see <http://www.gnu.org/licenses/>

#pragma once

//openCv
#include <cv.h>
#include <highgui.h>
#include <cxcore.h>
#include <stdlib.h>

class cAsfFile;

class cAsfPlayer
{
public:
    cAsfPlayer(cAsfFile &);
    ~cAsfPlayer();

    bool Play();

    void SetFullScreen(bool flag) { _full_screen = flag; }
    void SetScale(unsigned int value) { _scale = value; }
    void SetRecord(bool flag) { _record = flag; }
    bool GetFullScreen() const { return _full_screen; }
    cAsfFile& GetFile() const { return _file; };

    bool Init();
    bool RecordVideo();

private:
    cAsfFile &_file;
    bool _full_screen;
    bool _record;
    unsigned int _scale;
    unsigned int _frame;
    bool _pause;
    bool _exit_flag;
    struct timeval _frame_deadline;

    IplImage* _img;
    IplImage* _dst;
    CvCapture* _capture;
    uchar* _data;

    bool _InitRecord();
    void _ShowFrame();
    int _ProcessKey(int);
    bool _ControlKey();
    void _SetFirstTime(timeval &);
    int _GetWaitTime();
    void _GetWebCamData(std::vector<int> &);

    // Wait for user input atmost @msec milliseconds
    int _WaitForKey(int msec = 0);
};

// vim: set et ts=4 sw=4:
