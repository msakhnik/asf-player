#include "AsfPlayer.h"

#include <fstream>
#include <iostream>

using namespace std;

AsfPlayer::AsfPlayer(const string& file)
{
    this->filename = file;

    for (int i = 0; i < 200; i++)
        for (int j = 0; i < 200; i++)
            this->data_image[i][j] = 0;
}

bool AsfPlayer::show_image(unsigned frame)
{
    if (frame >= this->asf_file.start_frame)
    {

        uchar *data;

        IplImage *img = cvCreateImage(cvSize(asf_file.cols, asf_file.rows), IPL_DEPTH_8U, 1);

        data = (uchar *) img->imageData;

        for (int i = 0; i < (img->height) ; i++) {
            for (int j = 0; j < (img->width); j++) {
                data[i * (img->widthStep) + j * (img->nChannels)] = this->data_image[i][j];
            }
        }

        cvNamedWindow("frame", CV_WINDOW_NORMAL);

        if (this->full_screen)
            cvSetWindowProperty("frame", CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

        cvShowImage("frame", img);

        if (this->frame_by_frame) {
            cvWaitKey(0);
        } else {
            cvWaitKey(asf_file.seconds_per_frame);
        }

        cvReleaseImage(&img);

        if (frame == asf_file.end_frame) {
            cvDestroyWindow("image");
            return false;
        }
    }

    return true;
}

void AsfPlayer::readLine(string & str, vector<string> &array, char split)
{
    istringstream is(str);
    string s;

     while (getline(is, s, split))
        array.push_back(s);
}

bool AsfPlayer::readFile()
{
    string line;

    string str = this->filename;

    //convert string to *char
    char * writable = new char[str.size() + 1];
    copy(str.begin(), str.end(), writable);
    writable[str.size()] = '\0';

    ifstream myfile(writable);

    //flag for read data
    // if header is true - read header info
    // if header false - reaad frame data
    bool header = true;
    int frame = 0;
    int time = 0;
    int i = 0;

    if (myfile.is_open()) {
        while (myfile.good()) {
            getline(myfile, line);

            if(line.length()  <= 3) {

                if (frame) {
                    // create and show image
                    if (!show_image(frame))
                        break;
                }

                //initial frame data for geting new value
                frame = 0;
                time = 0;
                header = false;

                continue;
            }

            // read video info and save in global map-array
            if (header) {
                vector<string> tmp_array;

                readLine(line, tmp_array, ' ');
                string tmp_val;

                vector<string>::iterator iter = tmp_array.begin() + 1;

                for (iter = tmp_array.begin() + 1; iter != tmp_array.end(); iter++)
                    tmp_val += *iter;

                if (tmp_array[0] == "DATA_TYPE") {
                    asf_file.data_type = tmp_val;
                } else if (tmp_array[0] == "VERSION") {
                    asf_file.version = tmp_val;
                } else if (tmp_array[0] == "NOISE_THRESHOLD") {
                    asf_file.noise_threshold = atoi(tmp_val.c_str());
                } else if (tmp_array[0] == "COLS") {
                    asf_file.cols = atoi(tmp_val.c_str());
                } else if (tmp_array[0] == "ROWS") {
                    asf_file.rows = atoi(tmp_val.c_str());
                } else if (tmp_array[0] == "START_FRAME") {
                    asf_file.start_frame = atoi(tmp_val.c_str());
                } else if (tmp_array[0] == "END_FRAME") {
                    asf_file.end_frame = atoi(tmp_val.c_str());
                } else if (tmp_array[0] == "SECONDS_PER_FRAME") {
                    asf_file.seconds_per_frame =  1000*atof(tmp_val.c_str());
                } else if (tmp_array[0] == "ASCII_DATA") {
                    asf_file.ascii_data = tmp_val;
                } else {
                    asf_file.info[tmp_array[0]] = tmp_val;
                }
            } else {
                //if frame 0 then this line if title new frame.
                if (!frame) {

                    vector<string> tmp_array;
                    readLine(line, tmp_array, ',');

                    vector<string> tmp_array2;
                    readLine(tmp_array[0], tmp_array2, ' ');
                    frame = atoi(tmp_array2[1].c_str());

                    //if asf file without timestamp (example 1)
                    if (tmp_array.size() == 2) {
                        vector<string> tmp_array2;
                        readLine(tmp_array[1], tmp_array2, ' ');
                        time = atoi(tmp_array2[2].c_str());
                    }

                    cout << "frame:" << frame << "  timestamp:" << time << endl;
                    i = 0;

                    continue;
                } else {
                    // read pixels info
                    vector<string> tmp_array;

                    readLine(line, tmp_array, ',');

                    for (unsigned j = 0; j < asf_file.cols; ++j)
                    {
                        this->data_image[i][j] = atoi(tmp_array[j].c_str());
                    }

                    i++;
                }

            }
        }
        myfile.close();
    }
    else
        return false;


    return true;
}

// show all headers
void AsfPlayer::get_header()
{

    AsfFile::MapType::iterator iter = asf_file.info.begin();

    cout << endl << "FILE_NAME = " << this->filename<< endl;

    for (iter = asf_file.info.begin(); iter != asf_file.info.end(); iter++) {
        cout << "[" << (*iter).first << "] = " << (*iter).second << endl;
    }

}
