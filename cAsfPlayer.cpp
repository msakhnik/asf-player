#include "cAsfPlayer.h"
#include <iostream>

using namespace std;

cAsfPlayer::cAsfPlayer(const cAsfFile & file)
    : _file (file)
{

}

bool cAsfPlayer::Play()
{
    if (!_file.ReadHeader())
    {
        cout << "Error reading data\n";
        return false;
    }

    if (!_file.ReadFrame())
    {
        cout << "Error reading data\n";
        return false;
    }
    
    return true;
}