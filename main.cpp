#include <iostream>
#include "tga.h"

using namespace std;
using namespace TGA;

int main() {

    FILE *tga_file = fopen("24.tga", "r");

    TGA_Header header;

    fread(&header, sizeof(TGA_Header), 1, tga_file);

    u_int8_t data[header.image_spec.width * header.image_spec.height * header.image_spec.color_depth / 8];

    fread(&data, sizeof(data), 1, tga_file);

    TGA_Footer footer;

    fread(&footer, sizeof(TGA_Footer), 1, tga_file);

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

    cout << "Footer sign: " << (string(footer.tga_sign) == "TRUEVISION-XFILE." ? "TRUE" : "FALSE") << endl;

    auto image = TGA_Image::newImage(10, 5);

    image->setPixel(0, 0, Color24{0xFF, 0, 0});
    image->setPixel(9, 0, Color24{0, 0xFF, 0});
    image->setPixel(9, 4, Color24{0, 0, 0xFF});

    image->saveFile("out.tga");

    delete image;
    return 0;
}
