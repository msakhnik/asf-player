#include "AsfFile.h"

#include <iostream>
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

AsfFile::AsfFile(const string& file)
{
    
    this->filename = file;

    //initial header array
    this->info["HARDWARE"] = "0";
    this->info["HW_TYPE"] = "MOVIE";
    this->info["SENSOR_TYPE"] = "0";
    this->info["ROW_SPACING"] = "0";
    this->info["COL_SPACING"] = "0";
    this->info["SENSEL_AREA"] = "0";
    this->info["MICRO_SECOND"] = "0";
    this->info["TIME"] = "0";
    this->info["SENSITIVITY"] = "0";
    this->info["UNITS"] = "0";
    this->info["MIRROR_ROW"] = "0";
    this->info["MIRROR_COL"] = "0";
}


void AsfFile::Read_Line(string & str, vector<string> &array, char split)
{
    istringstream is(str);
    string s;

    while (getline(is, s, split))
        array.push_back(s);
}

bool AsfFile::Read_File()
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
    bool initialPlayerData = false;
    int frame = 0;
    int time = 0;
    int i = 0;

    if (myfile.is_open())
    {
        while (myfile.good())
        {

            getline(myfile, line);

            if (line.length() <= 3)
            {

                if (frame)
                {
                    if (!initialPlayerData)
                    {
                        player.cols = this->cols;
                        player.rows = this->rows;
                        player.seconds_per_frame = this->seconds_per_frame;
                        player.start_frame = this->start_frame;
                        player.end_frame = this->end_frame;
                    }
                    // create and show image
                    if (!player.Show_Image(frame, this->data_image))
                    {
                        this->data_image.erase(this->data_image.begin(), data_image.end());
                        break;
                    }

                    this->data_image.erase(this->data_image.begin(), data_image.end());
                }

                //initial frame data for geting new value
                frame = 0;
                time = 0;
                header = false;

                continue;
            }

            // read video info and save in global map-array
            if (header)
            {
                vector<string> tmp_array;

                Read_Line(line, tmp_array, ' ');
                string tmp_val;

                vector<string>::iterator iter = tmp_array.begin() + 1;

                for (iter = tmp_array.begin() + 1; iter != tmp_array.end(); iter++)
                    tmp_val += *iter;

                if (tmp_array[0] == "DATA_TYPE")
                    this->data_type = tmp_val;
                else if (tmp_array[0] == "VERSION")
                    this->version = tmp_val;
                else if (tmp_array[0] == "NOISE_THRESHOLD")
                    this->noise_threshold = atoi(tmp_val.c_str());
                else if (tmp_array[0] == "COLS")
                    this->cols = atoi(tmp_val.c_str());
                else if (tmp_array[0] == "ROWS")
                    this->rows = atoi(tmp_val.c_str());
                else if (tmp_array[0] == "START_FRAME")
                    this->start_frame = atoi(tmp_val.c_str());
                else if (tmp_array[0] == "END_FRAME")
                    this->end_frame = atoi(tmp_val.c_str());
                else if (tmp_array[0] == "SECONDS_PER_FRAME")
                    this->seconds_per_frame = 1000 * atof(tmp_val.c_str());
                else if (tmp_array[0] == "ASCII_DATA")
                    this->ascii_data = tmp_val;
                else
                    this->info[tmp_array[0]] = tmp_val;
            }
            else
            {
                //if frame 0 then this line if title new frame.
                if (!frame)
                {

                    vector<string> tmp_array;
                    Read_Line(line, tmp_array, ',');

                    vector<string> tmp_array2;
                    Read_Line(tmp_array[0], tmp_array2, ' ');
                    frame = atoi(tmp_array2[1].c_str());

                    //if asf file without timestamp (example 1)
                    if (tmp_array.size() == 2)
                    {
                        vector<string> tmp_array2;
                        Read_Line(tmp_array[1], tmp_array2, ' ');
                        time = atoi(tmp_array2[2].c_str());
                    }

                    cout << "frame:" << frame << "  timestamp:" << time << endl;
                    i = 0;

                    continue;
                }
                else
                {
                    // read pixels info
                    vector<string> tmp_array;

                    Read_Line(line, tmp_array, ',');

                    for (unsigned j = 0; j < this->cols; ++j)
                    {
                        this->data_image.push_back(atoi(tmp_array[j].c_str()));
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

void AsfFile::Get_Header()
{

    AsfFile::MapType::iterator iter = this->info.begin();

    cout << endl << "FILE_NAME = " << this->filename << endl;

    for (iter = this->info.begin(); iter != this->info.end(); iter++)
    {
        cout << "[" << (*iter).first << "] = " << (*iter).second << endl;
    }

}

void AsfFile::Set_Full_Screen()
{
    player.full_screen = true;
}

void AsfFile::Set_Frame_By_Frame()
{
    player.frame_by_frame = true;
}

bool AsfFile::Write_File()
{
    string file = "example/";
    int width = 160;
    int height = 120;

    file += this->filename;
    file += ".asf";

    ofstream out(file.c_str());

    if (!out)
    {
        cout << "Cannot open file.\n";
        return false;
    }

    out << "DATA_TYPE " << "MOVIE" << endl;
    out << "VERSION " << "NRI Capacitive 1.0-A" << endl;
    out << "SECONDS_PER_FRAME " << "0.1" << endl;
    out << "ROWS " << height << endl;
    out << "COLS " << width << endl;
    out << "NOISE_THRESHOLD " << 0 << endl;
    out << "START_FRAME " << 1 << endl;
    out << "ASCII_DATA " << "@@" << endl;
    out << "END_FRAME   " << player.Record_Video(data_image) << endl;

    int k = 0;
    int n= 1;
    int m = 0;
    int frame = 1;

    out << endl;
    out << "Frame " << frame << " timestamp 0" << endl;
    frame++;

      for (unsigned int i = 0; i < data_image.size() - 1; i++)
      {

          out << data_image[k];

          if (n == width)
          {
              out << endl;
              n = 0;
              m++;
          }
          else
              out << ",";

          if (m == height)
          {
              out << "" << endl;
              m = 0;

              out << "Frame " << frame << " timestamp 0" << endl;
              frame++;
          }

            k++;
            n++;
        }

    out.close();

    return true;
}