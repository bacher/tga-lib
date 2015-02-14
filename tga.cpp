#include <Tcl/tcl.h>
#include <ostream>
#include <iostream>
#include "tga.h"

using namespace std;

namespace TGA {

    TGA_Image::TGA_Image() {

    }

    void TGA_Image::saveFile(string fileName) {

        TGA_Header header;
        memset(&header, 0, sizeof(header));

        header.image_type = 2;
        header.image_spec.width = width;
        header.image_spec.height = height;
        header.image_spec.color_depth = 24;

        TGA_Footer footer;
        memset(&footer, 0, sizeof(footer));

        strcpy(footer.tga_sign, "TRUEVISION-XFILE.");

        FILE* save_file = fopen(fileName.c_str(), "w");

        fwrite(&header, sizeof(header), 1, save_file);
        fwrite(data, data_length, 1, save_file);
        fwrite(&footer, sizeof(footer), 1, save_file);

        fclose(save_file);

    }

    void TGA_Image::setPixel(u_int16_t x, u_int16_t y, Color24 color) {

        memcpy(&data[(width * y + x) * 3], &color, 3);

    }

    TGA_Image* TGA_Image::newImage(u_int16_t width, u_int16_t height) {

        auto tga_image = new TGA_Image();

        tga_image->width = width;
        tga_image->height = height;

        tga_image->data_length = (unsigned)(width * height * 3);
        tga_image->data = new u_int8_t[tga_image->data_length];

        memset(tga_image->data, 0, tga_image->data_length);

        return tga_image;

    }

}
