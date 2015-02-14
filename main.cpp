#include <iostream>
#include "tga.h"

using namespace std;
using namespace TGA;

int main() {

    auto image = TGA_Image::newImage(10, 5);

    image->setPixel(0, 0, Color24{0xFF, 0, 0});
    image->setPixel(9, 0, Color24{0, 0xFF, 0});
    image->setPixel(9, 4, Color24{0, 0, 0xFF});

    image->saveFile("out.tga");

    delete image;


    auto opened = TGA_Image::openImage("24.tga");

    opened->saveFile("out2.tga", true);

    delete opened;

    //TGA_Image::printInfo("24_rle.tga");

    return 0;
}
