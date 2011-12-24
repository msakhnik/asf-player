#include "AsfPlayer.h"
#include "AsfFile.h"

#include <iostream>
#include <sys/time.h>

using namespace std;

cAsfPlayer::cAsfPlayer(cAsfFile &file) : _file(file)
{
    SetFullScreen(false);
    SetFrameByFrame(false);

    _file.ReadHeader();

    img = cvCreateImage(cvSize(_file.GetCols(), _file.GetRows()),
                        IPL_DEPTH_8U, 1);

    data = (uchar *) img->imageData;

    cvNamedWindow("frame", CV_WINDOW_NORMAL);

}

cAsfPlayer::~cAsfPlayer()
{
    cvReleaseImage(&img);
    cvDestroyWindow("image");
}

bool cAsfPlayer::Play()
{
    //Читаю і показую по фреймові
    if (this->_ShowFrame())
        return true;
    else
    {
        cerr << "Error reading frames" << endl;
        return false;
    }
}

bool cAsfPlayer::_ShowFrame()
{
    vector<int> data_image;

    //Отримую перший фрейм
    data_image = this->_file.ReadFrame();

    //Якщо пустий рядок це виходжу
    if (data_image.empty())
        return false;

    //Ініціалізація вікна для відтворення даних

    for (unsigned int frame = this->_file.GetStartFrame();
        frame <= this->_file.GetEndFrame(); ++frame)
    {
        int k = 0;

        cout << "Frame: " << frame << endl;

        for (int i = 0; i < (img->height); ++i)
        {
            for (int j = 0; j < (img->width); ++j)
            {
                data[i * (img->widthStep) + j *
                        (img->nChannels)] = data_image[k];
                ++k;
            }
        }

        if (this->GetFullScreen())
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN,
                                CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", img);

        //Тут запам'ятовую час для зчитування наступного кадра
        struct timeval t0;
        if (!this->GetFrameByFrame())
            gettimeofday(&t0, NULL);

        data_image.clear();
        data_image = this->_file.ReadFrame();

        if (!this->GetFrameByFrame())
        {
            struct timeval t1;
            gettimeofday(&t1, NULL);

            int time = (double) (t1.tv_usec - t0.tv_usec) / 1000;

            //Якщо зчитувалось довше ніж seconds_per_frame (затримка перед
            //наступним кадром) то виводжу повідомлення про повільне зчитування
            //інакше віднімаю отриманий час
            if (time >= this->_file.GetSecondsPerFrame())
            {
                cout << "Slow playing..." << endl;
            }
            else
            {
                cvWaitKey(this->_file.GetSecondsPerFrame() - time);
            }
        }
        else
            cvWaitKey(0);
    }

    return true;
}