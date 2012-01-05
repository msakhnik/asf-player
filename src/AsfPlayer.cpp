//
// AsfPlayer.cpp
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

#include "AsfPlayer.h"
#include "AsfFile.h"

#include <iostream>
#include <stdlib.h>
#include <sys/time.h>

using namespace std;

cAsfPlayer::cAsfPlayer(cAsfFile &file)
    : _file(file)
    , _full_screen(false)
    , _frame_by_frame(false)
    , _track(false)
    , _scale(1)
{
}

cAsfPlayer::~cAsfPlayer()
{
    cvReleaseImage(&_img);
    cvReleaseImage(&_dst);
    cvDestroyWindow("image");
}

bool cAsfPlayer::Init()
{
    if (!_file.ReadHeader() || !_file.SetPositionFrame())
    {
        cerr << "Error reading from file" << endl;
        return false;
    }
    if (!_file.GetCols() || !_file.GetRows())
    {
        cerr << "Can't reproduce one-dimensional film" << endl;
        return false;
    }
    _img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                         IPL_DEPTH_8U, 1);
     _dst = cvCreateImage(cvSize(_img->width*_scale,
                                         _img->height * _scale),
                                  _img->depth, _img->nChannels);
    if (!_img || !_dst)
    {
        cerr << "Cannot create a window " << endl;
        return false;
    }
    _data = reinterpret_cast<uchar *>(_img->imageData);
    cvNamedWindow("frame", _full_screen ? 0 : 1);
    // Check if it's the full screen mode
    if (this->_full_screen)
        cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                            CV_WINDOW_FULLSCREEN);
    return true;
}

bool cAsfPlayer::Play()
{
    if (this->_ShowFrame())
    {
        cout << endl;
        return true;
    }

    cerr << "Error reading frames" << endl;
    return false;
}

void cAsfPlayer::_FillImgData()
{
    cAsfFile::FrameT const& image_data = _file.GetLastFrame();
    vector<int>::const_iterator iter = image_data.begin();

    for (int i = 0; i < _img->height; ++i)
    {
        for (int j = 0; j < _img->width; ++j)
        {
            _data[i * _img->widthStep + j * _img->nChannels]
                = *(++iter);
        }
    }
}

void cAsfPlayer::_SetPlayerOptions(unsigned int &frame, unsigned int & end)
{
    if (_scale)
    {
        _dst = cvCreateImage (cvSize (_img->width*_scale,
                                               _img->height*_scale),
                                       _img->depth, _img->nChannels);
        cvResize(_img, _dst, 1);
    }
}

int cAsfPlayer::_ProcessKey(int key, unsigned int & frame, bool & pause)
{
    int exit_flag = 2;
    switch (key)
    {
    case -1: // Timeout
        exit_flag = 0; //repeat
        break;
    case 'q':
        cout << "\nBye!" << endl;
        exit_flag = 1;//good exit
        break;
    case ',':
        if (!pause)
            pause = true;
        frame -= 2;
        _file.ChangePosition(frame);
        break;
    case '.':
        if (!pause)
            pause = true;
        ++frame;
        _file.ChangePosition(frame);
        break;
    case ' ':
        pause = !pause;
        break;
    default:
        exit_flag = 0;
        break;
    }
    return exit_flag;
}

void cAsfPlayer::_SetFirstTime(timeval& frame_deadline, timeval& frame_duration)
{
    gettimeofday(&frame_deadline, NULL); // Show the first frame right now
    timeradd(&frame_deadline, &frame_duration, &frame_deadline);
}

int cAsfPlayer::_GetWaitTime(timeval& frame_deadline)
{
    struct timeval now;
    gettimeofday(&now, NULL);
    int wait_time = 1; //minimum delay
    if (timercmp(&now, &frame_deadline, <))
    {
        struct timeval new_time;
        new_time.tv_sec = 0;
        new_time.tv_usec = 0;
        timersub(&frame_deadline, &now, &new_time);
        wait_time = new_time.tv_sec * 1000 +
            new_time.tv_usec / 1000;
    }
    else
        cerr << "\nSlow playing..." << endl;
    return wait_time;
}

bool cAsfPlayer::_ControlKey(unsigned int & frame, timeval & frame_deadline)
{
    int key = 0;
    static bool pause = false;
    if (pause || _frame_by_frame)
        while (!(key = _ProcessKey(cvWaitKey(0), frame, pause)))
        {
        }
    else
        key = _ProcessKey(cvWaitKey(_GetWaitTime(frame_deadline)),
                                                        frame, pause);
    if (key == 1)
        return true;

    return false;
}

bool cAsfPlayer::_ShowFrame()
{
    unsigned int end_frame = this->_file.GetEndFrame();
    struct timeval frame_deadline, frame_duration;

    int delay = _file.GetMsecPerFrame() * 1000;
    frame_duration.tv_sec = 0; frame_duration.tv_usec = delay;
    
    for (unsigned frame = this->_file.GetStartFrame();
                        frame <= end_frame; ++frame)
    {
        cout << "\rFrame: " << frame << flush;
        _SetFirstTime(frame_deadline, frame_duration);
        _file.ReadFrame();
        _FillImgData();
        _SetPlayerOptions(frame, end_frame);

        cvShowImage("frame", _full_screen ? _img : _dst);

        if (_ControlKey(frame, frame_deadline))
            break;
    }
return true;
}

// vim: set et ts=4 sw=4:
