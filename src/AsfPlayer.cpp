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
    , _record(false)
    , _scale(1)
    , _frame(0)
    , _pause(false)
    ,  _exit_flag(false)
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
    if (_full_screen)
        cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                            CV_WINDOW_FULLSCREEN);
    return true;
}

bool cAsfPlayer::Play()
{
    unsigned int end_frame = _file.GetEndFrame();
    struct timeval frame_duration;

    int delay_usec = _file.GetMsecPerFrame() * 1000;
    frame_duration.tv_sec = delay_usec / 1000000;
    frame_duration.tv_usec = delay_usec % 1000000;

    for (_frame = _file.GetStartFrame();
         _frame <= end_frame && !_exit_flag;
         ++_frame)
    {
        cout << "\rFrame: " << _frame << flush;
        _SetFirstTime(frame_duration);
        if (!_ShowFrame())
        {
            cerr << "Cannot show a frame" << endl;
            return false;
        }

        _WaitForInput();
    }
    cout << "\nBye!" << endl;
    return true;
}

// Returns true if timeout elapsed or next frame has to be shown
bool cAsfPlayer::_ProcessKey(int key)
{
    // FIXME: Apparently, it doesn't work when NumLock is on.
    cout << key << endl;
    switch (key)
    {
    case -1: // Timeout
        break;
    case 'q':
        _exit_flag = true;
        break;
    case ',':
        if (!_pause)
            _pause = true;
        if (_frame > 1)
        {
            _frame -= 2;
            _file.ChangePosition(_frame + 1);
        }
        else
            return false;
        break;
    case '.':
        if (!_pause)
            _pause = true;
        if (_frame != _file.GetEndFrame())
            _file.ChangePosition(_frame+1);
        break;
    case ' ':
        _pause = !_pause;
        break;
    default:
        return false;
    }
    return true;
}

void cAsfPlayer::_SetFirstTime(timeval& frame_duration)
{
    gettimeofday(&_frame_deadline, NULL); // Show the first frame right now
    timeradd(&_frame_deadline, &frame_duration, &_frame_deadline);
}

int cAsfPlayer::_GetWaitTime()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    struct timeval new_time;
    new_time.tv_sec = 0;
    new_time.tv_usec = 0;
    timersub(&_frame_deadline, &now, &new_time);
    int wait_time = new_time.tv_sec * 1000 +
                    new_time.tv_usec / 1000;
    return wait_time;
}

// Process user input until the next frame has to be shown.
void cAsfPlayer::_WaitForInput()
{
    // The user may press a key, thus, interrupting the waiting.
    // So we need to retry waiting until the next frame has to be shown.
    for (int i = 0; ; ++i)
    {
        int wait_msec = 0;
        if (!_pause)
        {
            wait_msec = _GetWaitTime();
            if (wait_msec <= 0)
            {
                if (i > 0)
                    return;
                cerr << "\nYour system is too slow..." << endl;
                wait_msec = 1; // minimal wait time
            }
        }

        int key = cvWaitKey(wait_msec);
        if (_ProcessKey(key))
            return;
    }
}

bool cAsfPlayer::_ShowFrame()
{
    if (!_file.ReadFrame())
    {
        // FIXME: TBD
    }
    cAsfFile::FrameT const& image_data = _file.GetLastFrame();
    vector<int>::const_iterator iter = image_data.begin();

    for (int i = 0; i < _img->height; ++i)
        for (int j = 0; j < _img->width; ++j)
            _data[i * _img->widthStep + j * _img->nChannels]= *(++iter);

    if (_scale)
    {
        _dst = cvCreateImage(cvSize (_img->width*_scale,
                                           _img->height*_scale),
                                           _img->depth, _img->nChannels);
        cvResize(_img, _dst, 1);
        if (!_dst)
            return false;
    }
    if (!_img)
        return false;
    cvShowImage("frame", _full_screen ? _img : _dst);
    return true;
}

bool cAsfPlayer::_InitRecord()
{
    if (!_file.InitRecordFile())
        return false;
    _capture = cvCreateCameraCapture(-1);
    assert(_capture);

    if (!_capture)
    {
        cout << "Webcam not found!" << endl;
        return false;
    }
    cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, 320); //1280);
    cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, 240); //960);

    _file.SetCols(cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH));
    _file.SetRows(cvGetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT));

    cvNamedWindow("image", CV_WINDOW_AUTOSIZE);

    return true;
}

void cAsfPlayer::_GetWebCamData(vector<int> & data_image)
{
    static int width = _file.GetCols();
    static int height = _file.GetRows();
    _img = cvQueryFrame(_capture);
    _dst = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
        //Переводжу в відтінки сірого
        cvCvtColor(_img, _dst, CV_RGB2GRAY);

    for (int i = 0; i < _dst->height; i++)
        for (int j = 0; j < _dst->width; j++)
            data_image.push_back(cvGetReal2D(_dst, i, j));
}

bool cAsfPlayer::RecordVideo()
{
    if (!_InitRecord())
    {
        cvReleaseCapture(&_capture);
        return false;
    }
    vector<int> data_image;
    _frame = 1;
    while (true)
    {
        data_image.clear();
        _GetWebCamData(data_image);
        _file.RecordFrame(data_image, _frame);

        cvShowImage("image", _dst);

        char c = cvWaitKey(33);
        if (c == 'q')
        {
            cvReleaseCapture(&_capture);
            break;
        }
        ++_frame;
    }
    _file.SetEndFrame(_frame);
    if (!_file.RecordHeader())
        return false;
    cerr << endl;
    return true;
}

// vim: set et ts=4 sw=4:
