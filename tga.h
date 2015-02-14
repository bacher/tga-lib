#include <i386/types.h>
#include <Python/Python.h>

using namespace std;

namespace TGA {

    union Color24 {
        u_int8_t rgb[3];
        u_int32_t color;
    };

    struct TGA_Header__Image_Spec {
        u_int16_t pos_x;
        u_int16_t pos_y;
        u_int16_t width;
        u_int16_t height;
        u_int8_t color_depth;
        u_int8_t description;
    };

    struct TGA_Header {
        u_int8_t id_length;
        u_int8_t color_map_type;
        u_int8_t image_type;
        u_int8_t color_map_spec[5];
        TGA_Header__Image_Spec image_spec;
    };

    struct TGA_Footer {
        u_int32_t expansion_offset:24;
        u_int64_t developer_offset:40;
        char tga_sign[18];
    };

    class TGA_Image {
    public:
        static TGA_Image* newImage(u_int16_t width, u_int16_t height);
        static TGA_Image* openImage(string path);
        static void printInfo(string path);

        void setPixel(u_int16_t x, u_int16_t y, Color24);
        void saveFile(string);

        TGA_Image();
    protected:

    private:
        u_int16_t width;
        u_int16_t height;
        u_int8_t depth;

        u_int8_t* data = nullptr;
        u_int32_t data_length;

    };

}
