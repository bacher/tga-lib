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
        header.image_spec.color_depth = depth;

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
        tga_image->depth = 24;

        tga_image->data_length = (unsigned)(width * height * tga_image->depth / 3);
        tga_image->data = new u_int8_t[tga_image->data_length];

        memset(tga_image->data, 0, tga_image->data_length);

        return tga_image;

    }

    TGA_Image* TGA_Image::openImage(string path) {

        auto tga_image = new TGA_Image();

        FILE* tga_file = fopen(path.c_str(), "r");

        TGA_Header header;

        fread(&header, sizeof(TGA_Header), 1, tga_file);

        if (header.image_type != 2) {
            throw "Allow only image-type 2.";
        }

        if (header.image_spec.color_depth != 24) {
            throw "Allow only 24bit color.";
        }

        tga_image->width = header.image_spec.width;
        tga_image->height = header.image_spec.height;
        tga_image->depth = header.image_spec.color_depth;

        tga_image->data_length = (unsigned)(tga_image->width * tga_image->height * tga_image->depth / 8);
        tga_image->data = new u_int8_t[tga_image->data_length];

        fread(tga_image->data, tga_image->data_length, 1, tga_file);

        TGA_Footer footer;

        fread(&footer, sizeof(TGA_Footer), 1, tga_file);

        fclose(tga_file);

        return tga_image;
    }

    void TGA_Image::printInfo(string path) {

        FILE* tga_file = fopen(path.c_str(), "r");

        TGA_Header header;

        fread(&header, sizeof(TGA_Header), 1, tga_file);

        fclose(tga_file);

        cout << "id_length: " << (int)header.id_length << endl;
        cout << "color_map_type: " << (int)header.color_map_type << endl;
        cout << "image_type: " << (int)header.image_type << endl;
        cout << "color_map_spec: " <<
                (int)header.color_map_spec[0] <<
                (int)header.color_map_spec[1] <<
                (int)header.color_map_spec[2] <<
                (int)header.color_map_spec[3] <<
                (int)header.color_map_spec[4] <<
                endl;

        cout << "image_spec:" << endl;
        cout << "  x,y: " << header.image_spec.pos_x << ',' << header.image_spec.pos_y << endl;
        cout << "  width,height: " << header.image_spec.width << ',' << header.image_spec.height << endl;
        cout << "  color_depth: " << (int)header.image_spec.color_depth << endl;
        cout << "  description: " << (int)header.image_spec.description << endl;

        //cout << "Footer sign: " << (string(footer.tga_sign) == "TRUEVISION-XFILE." ? "TRUE" : "FALSE") << endl;
    }

}
