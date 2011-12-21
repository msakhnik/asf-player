#include "cAsfPlayer.h"
#include <iostream>

using namespace std;

cAsfPlayer::cAsfPlayer(const cAsfFile & file)
: _file(file)
{}

bool cAsfPlayer::Play()
{
//Читаю заголовок
    if (!_file.ReadHeader())
    {
        cout << "Error reading data\n";
        return false;
    }

    //Читаю і показую по фреймові
   if (_ShowFrame())
       return true;
   else
   {
       cout << "Error reading data\n";
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
    uchar *data;
    IplImage *img = cvCreateImage(cvSize(this->_file.cols, this->_file.rows), IPL_DEPTH_8U, 1);
    data = (uchar *) img->imageData;

    cvNamedWindow("frame", CV_WINDOW_NORMAL);


    for (unsigned int frame = this->_file.start_frame; frame <= this->_file.end_frame; frame++)
    {
        int k = 0;

        cout << "Frame: " << frame << endl;

        for (int i = 0; i < (img->height); i++)
        {
            for (int j = 0; j < (img->width); j++)
            {
                data[i * (img->widthStep) + j * (img->nChannels)] = data_image[k];
                k++;
            }
        }

        cvShowImage("frame", img);

        //Тут запам'ятовую час для зчитування наступного кадра
        clock_t t0 = clock();

        data_image.clear();
        data_image = this->_file.ReadFrame();

        clock_t t1 = clock();
        
        int time = (double)(t1 - t0) / CLOCKS_PER_SEC * 1000;

        //Якщо зчитувалось довше ніж seconds_per_frame (затримка перед наступним кадром) то виводжу повідомлення про повільне зчитування
        //інакше віднімаю отриманий час
        if (time >= this->_file.seconds_per_frame)
        {
            cout << "Slow playing...\n";
        }
        else
        {
            cvWaitKey(this->_file.seconds_per_frame - time);
        }

    }

    cvReleaseImage(&img);
    cvDestroyWindow("image");

    return true;
}