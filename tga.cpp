#include <Tcl/tcl.h>
#include <ostream>
#include <iostream>
#include "tga.h"

using namespace std;

namespace TGA {

    void compress(u_int8_t* data, u_int32_t data_length, FILE* save_file) {

//        u_int8_t dt[8]{2, 2, 3, 2,  2, 2, 4, 2};
//
//        data = dt;
//        size = 8;

        u_int8_t buffer[data_length * 2];
        unsigned int b_index = 0;

        int size = data_length;

        int cnt = 1;
        int diff = 1;

        for (int i = 3; i <= size; i += 3) {

            if (i != size && data[i] == data[i - 3] && data[i + 1] == data[i - 2] && data[i + 2] == data[i - 1]) {
                cnt++;

                if (diff > 1) {
                    cout << '(' << diff << ") ";
                    buffer[b_index++] = (u_int8_t)(diff - 2);

                    for (int j = i - (diff * 3); j < i - 3; j += 3) {
                        //cout << (int)data[j] << ' ';
                        buffer[b_index++] = data[j];
                        buffer[b_index++] = data[j + 1];
                        buffer[b_index++] = data[j + 2];
                    }

                    diff = 1;
                }
            } else {
                if (cnt > 1) {
                    cout << cnt << '*' << (int)data[i - 3] << ' ';

                    int max = 128;

                    while (cnt > 0) {
                        buffer[b_index++] = 127 + (cnt > max ? max : cnt);
                        buffer[b_index++] = data[i - 3];
                        buffer[b_index++] = data[i - 2];
                        buffer[b_index++] = data[i - 1];

                        cnt -= max;
                    }
                    cnt = 1;
                    diff = 1;

                } else {

                    if (i == size) {

                        cout << '(' << diff << ") ";
                        buffer[b_index++] = diff;

                        for (int j = i - (diff * 3); j < i; j += 3) {
                            cout << (int)data[j] << ' ';
                            buffer[b_index++] = data[j];
                            buffer[b_index++] = data[j + 1];
                            buffer[b_index++] = data[j + 2];
                        }
                    } else {
                        diff++;
                    }
                }
            }

        }

        fwrite(buffer, b_index, 1, save_file);
    };

    TGA_Image::TGA_Image() {

    }

    TGA_Image::~TGA_Image() {
        delete data;
    }

    void TGA_Image::saveFile(string fileName, bool rle) {

        TGA_Header header;
        memset(&header, 0, sizeof(header));

        header.image_type = (u_int8_t)(rle ? 10 : 2);
        header.image_spec.width = width;
        header.image_spec.height = height;
        header.image_spec.color_depth = depth;

        TGA_Footer footer;
        memset(&footer, 0, sizeof(footer));

        strcpy(footer.tga_sign, "TRUEVISION-XFILE.");

        FILE* save_file = fopen(fileName.c_str(), "w");

        fwrite(&header, sizeof(header), 1, save_file);


        if (rle) {
            compress(data, data_length, save_file);
        } else {
            fwrite(data, data_length, 1, save_file);
        }

        fwrite(&footer, sizeof(TGA_Footer), 1, save_file);

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
            cerr << "Allow only image-type 2." << endl;
            throw;
        }

        if (header.image_spec.color_depth != 24) {
            cerr << "Allow only 24bit color." << endl;
            throw;
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
