#include <Tcl/tcl.h>
#include <ostream>
#include <iostream>
#include "tga.h"

using namespace std;

namespace TGA {

    typedef unsigned int uint;

    TGA_Image::TGA_Image() {

    }

    TGA_Image::TGA_Image(u_int16_t width, u_int16_t height, Color24 bg)
            :width(width), height(height) {

        data_length = (unsigned)(width * height * 3);
        data = new u_int8_t[data_length];

        for (int i = 0; i < width * height; ++i) {
            memcpy(data + i, &bg, 3);
        }
    }

    TGA_Image::~TGA_Image() {
        delete[] data;
    }

    TGA_Image* TGA_Image::openImage(string path) {

        auto tga_image = new TGA_Image();

        FILE* tga_file = fopen(path.c_str(), "r");

        TGA_Header header;

        fread(&header, sizeof(TGA_Header), 1, tga_file);

        if (header.image_type != 2 && header.image_type != 10) {
            cerr << "Allow only image-type 2 and 10." << endl;
            throw;
        }

        if (header.image_spec.color_depth != 24) {
            cerr << "Allow only 24bit color." << endl;
            throw;
        }

        tga_image->width = header.image_spec.width;
        tga_image->height = header.image_spec.height;

        tga_image->data_length = (unsigned)(tga_image->width * tga_image->height * 3);
        tga_image->data = new u_int8_t[tga_image->data_length];

        if (header.image_type == 2) {
            fread(tga_image->data, tga_image->data_length, 1, tga_file);
        } else {
            tga_image->decompress_data(tga_file);
        }

        TGA_Footer footer;

        fread(&footer, sizeof(TGA_Footer), 1, tga_file);

        fclose(tga_file);

        return tga_image;
    }

    void TGA_Image::saveFile(string fileName, bool rle) {

        TGA_Header header;
        memset(&header, 0, sizeof(header));

        header.image_type = (u_int8_t)(rle ? 10 : 2);
        header.image_spec.width = width;
        header.image_spec.height = height;
        header.image_spec.color_depth = 24;

        TGA_Footer footer;
        memset(&footer, 0, sizeof(footer));

        strcpy(footer.tga_sign, "TRUEVISION-XFILE.");

        FILE* save_file = fopen(fileName.c_str(), "w");

        fwrite(&header, sizeof(header), 1, save_file);

        if (rle) {
            compress_data(save_file);
        } else {
            fwrite(data, data_length, 1, save_file);
        }

        fwrite(&footer, sizeof(TGA_Footer), 1, save_file);

        fclose(save_file);

    }

    void TGA_Image::setPixel(u_int16_t x, u_int16_t y, Color24 color) {

        memcpy(&data[(width * y + x) * 3], &color, 3);

    }

    void TGA_Image::decompress_data(FILE* file) {

        Color24 color;
        u_int8_t rle_var;
        u_int8_t depth_bytes = 3;
        unsigned int d_index = 0;

        while (d_index < data_length) {

            fread(&rle_var, 1, 1, file);

            if (rle_var > 127) {
                int repeat_count = rle_var - 127;

                fread(&color, depth_bytes, 1, file);

                while (repeat_count--) {
                    memcpy(data + d_index, &color, depth_bytes);

                    d_index += depth_bytes;
                }

            } else {

                u_int32_t series_count = rle_var + 1;

                fread(data + d_index, series_count * depth_bytes, 1, file);

                d_index += series_count * depth_bytes;
            }
        }

        if (d_index != data_length) {
            cerr << "Rle decoding error." << endl;
            throw;
        }

    }

    void fill_diff(u_int8_t* buffer, uint* b_index, u_int8_t* data, int d_index, uint diff, u_int8_t depth_bytes) {

        uint max = 128;
        uint remain = diff;

        while (remain) {

            uint pass = (remain > max ? max : remain);

            buffer[(*b_index)++] = (u_int8_t)(pass - 1);

            for (
                    int i = 0, offset = d_index - (remain * depth_bytes);
                    i < pass;
                    i++, offset += depth_bytes
            ) {
                memcpy(buffer + (*b_index), data + offset, depth_bytes);

                *b_index += depth_bytes;
            }

            remain -= pass;

        }

    }

    void TGA_Image::compress_data(FILE* save_file) {

        u_int8_t buffer[data_length * 2];
        uint b_index = 0;
        u_int8_t depth_bytes = 3;

        int cnt = 1;
        int diff = 1;

        for (int i = depth_bytes; i <= data_length; i += depth_bytes) { // [1;length]

            if (i != data_length && memcmp(data + i, data + i - depth_bytes, depth_bytes) == 0) {
                cnt++;

                if (diff > 1) {

                    fill_diff(buffer, &b_index, data, i - depth_bytes, (uint)(diff - 1), depth_bytes);

                    diff = 1;
                }

            } else {
                if (cnt > 1) {
                    int max = 128;

                    while (cnt > 0) {
                        buffer[b_index++] = (u_int8_t)(127 + (cnt > max ? max : cnt));

                        memcpy(buffer + b_index, data + i - depth_bytes, depth_bytes);

                        b_index += depth_bytes;

                        cnt -= max;

                    }
                    cnt = 1;
                    diff = 1;

                } else {

                    if (i == data_length) {

                        fill_diff(buffer, &b_index, data, i, (uint)diff, depth_bytes);

                    } else {
                        diff++;
                    }
                }
            }

        }

        fwrite(buffer, b_index, 1, save_file);
    };

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
