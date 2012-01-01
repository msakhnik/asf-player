#include "AsfPlayer.h"
#include "AsfFile.h"

#include <iostream>
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

    if (!_img)
    {
        cerr << "Cannot create a window " << endl;
        return false;
    }

    _data = reinterpret_cast<uchar *>(_img->imageData);

    cvNamedWindow("frame", _full_screen ? 0 : 1);

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

// FIXME: This function is too larse, split it!
bool cAsfPlayer::_ShowFrame()
{
    //Get first frame
    if (!_file.ReadFrame())
        return false;

    unsigned int start = this->_file.GetStartFrame();
    unsigned int end = this->_file.GetEndFrame();

    //Need for look change frame on skip event;
    unsigned int check_frame = start;

    bool pause = false; //for play/pause

    for (unsigned int frame = start;
        frame <= end; ++frame)
    {
        cout << "\rFrame: " << frame << flush;

        //time how we get when reading additionally Frame
        int adition_time = 0;
        //If we changed position
        if ( frame - check_frame != 0)
        {
            struct timeval t0;
            gettimeofday(&t0, NULL);

            if (!_file.ReadFrame() && frame != end)
                cerr << "\nSome data is lost" << endl;

            struct timeval t1;
            gettimeofday(&t1, NULL);

            adition_time = (t1.tv_usec - t0.tv_usec) / 1000;

        }

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

        //put trackbar in top
        if (_track)
            cvCreateTrackbar2("Position", "frame", reinterpret_cast<int*>
                (&frame), static_cast<int>(end), TrackbarCallback, this);

        // Check if it's the full screen mode
        if (this->_full_screen)
        {
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);
            cvShowImage("frame", _img);
        }
        else
        {
            // FIXME: Handle allocation errors!
            IplImage* dst = cvCreateImage (cvSize (_img->width*_scale,
                                                   _img->height*_scale),
                                           _img->depth, _img->nChannels);
            cvResize(_img, dst, 1);

            cvShowImage("frame", dst);

            cvReleaseImage(&dst);
        }

        // Remember the moment to read the next frame
        struct timeval t0;
        if (!_frame_by_frame)
            gettimeofday(&t0, NULL);

        if (!_file.ReadFrame() && frame != end)
        {
            cerr << "\nSome data is lost" << endl;
        }

        // How many milliseconds to wait while the frame is being exposed
        int wait_time (0);

        struct timeval t1;
        if (!_frame_by_frame)
        {
            gettimeofday(&t1, NULL);
            // FIXME: Take into account .tv_sec and other fields.
            // Because when a lag occurs on a second overfull, usec
            // start counting from 0. In fact, I've never seen the message
            // "Slow playing" so far.
            int read_time = (t1.tv_usec - t0.tv_usec) / 1000;

            if (!pause)
                wait_time = _file.GetMsecPerFrame() - read_time - adition_time;

            if (wait_time < 0)
            {
                cout << "\nSlow playing..." << endl;
                wait_time = 1;
            }
        }

        // User input
        int key = cvWaitKey(wait_time);

        switch (key)
        {
        case -1: // Timeout
            break;
        case 'q':
            cout << "\nBye!" << endl;
            return true;
        case '.':
            if (!pause)
                pause = true;
            TrackbarCallback(frame, this);
            break;
        case ' ':
            pause = !pause;
            break;
        default:
            // FIXME: So we may underexpose a frame if user crushes
            // the keyboard. Implement absolute time point for frames.
            break;
        }

        ++check_frame;
    }

    return true;
}

// vim: set et ts=4 sw=4:
