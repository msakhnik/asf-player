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

    unsigned int window_size = 0;

    if (!this->_full_screen)
        window_size = 1;

    cvNamedWindow("frame", window_size);

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
            // FIXME: Fullscreen mode doesn't work any more.
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);
            cvShowImage("frame", _img);
        }
        else
        {
            // FIXME: Well yeah, it's created. But who will check if it's
            // actually created, and who will destroy it???
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

        if (key == 113) // "q"
        {
            cout << "\nBye!" << endl;
            return true;
        }
        else if (key == 46) // "."
        {
            TrackbarCallback(frame, this);
        }
        else if (key == 32) // "space"
        {
            if (pause)
                pause = false;
            else
                pause = true;
        }

        ++check_frame;
    }

    return true;
}

// vim: set et ts=4 sw=4:
