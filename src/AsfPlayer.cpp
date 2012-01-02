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
    if (!_file.ReadHeader())
    {
        cerr << "Error reading from file" << endl;
        return false;
    }

    if (!_file.SetPositionFrame())
    {
        cerr << "Error reading from file" << endl;
        return false;
    }

    int w = _file.GetCols();
    int h = _file.GetRows();

    //check the width and height
    if (!w || !h)
    {
        cerr << "Can't reproduce one-dimensional film" << endl;
        return false;
    }

    _img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                         IPL_DEPTH_8U, 1);

    IplImage* _dst = cvCreateImage(cvSize(_img->width*_scale,
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

static void TrackbarCallback(int pos, void* obj)
{
    static cAsfPlayer* playerObj = (cAsfPlayer*) obj;
    if (pos >= static_cast<int>(playerObj->GetFile().GetStartFrame())
            && pos <= static_cast<int>(playerObj->GetFile().GetEndFrame()))
    {
        playerObj->GetFile().ChangePosition(pos);
    }
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
                = *(iter++);
        }
    }
}

void cAsfPlayer::_SetPlayerOptions(unsigned int &frame, unsigned int & end)
{
    //put trackbar in top
    if (_track)
        cvCreateTrackbar2("Position", "frame", reinterpret_cast<int*>
                          (&frame), static_cast<int> (end), TrackbarCallback, this);
    if (_scale)
    {
        _dst = cvCreateImage(cvSize(_img->width*_scale,
                                    _img->height * _scale),
                             _img->depth, _img->nChannels);
        cvResize(_img, _dst, 1);
    }
}

int cAsfPlayer::_ProcessKey(int key, unsigned int & frame, bool & pause)
{
    int exit_flag = 0;
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
        TrackbarCallback(frame, this);
        exit_flag = 2; //do next
        break;
    case '.':
        if (!pause)
            pause = true;
        TrackbarCallback(frame, this);
        exit_flag = 2; //do next
        break;
    case ' ':
        pause = !pause;
        exit_flag = 2;
        break;
    default:
        exit_flag = 0;
        break;
    }

    return exit_flag;
}

void cAsfPlayer::_CheckFrame()
{
    if (!_file.ReadFrame())
        cerr << "\nSome data is lost" << endl;
}

bool cAsfPlayer::_ShowFrame()
{
    unsigned int frame = this->_file.GetStartFrame()
                        , end_frame = this->_file.GetEndFrame()
                        , check_position = this->_file.GetStartFrame() - 1;
                        ;
    bool pause = false; //for play/pause
    int delay = _file.GetMsecPerFrame() * 1000;

    struct timeval frame_duration;
    frame_duration.tv_sec = 0;
    frame_duration.tv_usec = delay;

    _CheckFrame();

    while (frame <= end_frame)
    {
        struct timeval frame_deadline;
        gettimeofday(&frame_deadline, NULL); // Show the first frame right now

        timeradd(&frame_deadline, &frame_duration, &frame_deadline);

        if (frame - check_position != 1)
        {
            _CheckFrame();
            check_position = frame;
        }

        _FillImgData();
        _SetPlayerOptions(frame, end_frame);

        cvShowImage("frame", _full_screen ? _img : _dst);

        if (frame != end_frame)
            _CheckFrame();

        struct timeval now;
        gettimeofday(&now, NULL);

        int wait_time = 1; //minimum delay

        if (timercmp(&now, &frame_deadline, <))
        {
            struct timeval new_time;

            timersub(&frame_deadline, &now, &new_time);
            wait_time = frame_duration.tv_sec * 1000 +
                                frame_duration.tv_usec / 1000;
        }
        else
            cerr << "\nSlow playing..." << endl;

        int key = 0;

        if (pause || _frame_by_frame)
            while(!(key = _ProcessKey(cvWaitKey(0), frame, pause)))
            {
            }
        else
            key = _ProcessKey(cvWaitKey(wait_time), frame, pause);

        if (key == 1)
            return true;

        ++check_position;
        ++frame;
    }

return true;
}

// vim: set et ts=4 sw=4:
