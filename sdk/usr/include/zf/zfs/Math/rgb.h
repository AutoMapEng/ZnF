#pragma once
#include <cstdint>

namespace zfs 
{
    namespace math
    {
#pragma pack(1)
        /*
        \brief Holds rgb value 
        */
        class rgb
        {
        public:

            rgb() {}
            rgb(unsigned char r_, unsigned char g_, unsigned char b_) : red(r_), green(g_), blue(b_) {}
            rgb(int r_, int g_, int b_) : red(static_cast<unsigned char>(r_)), green(static_cast<unsigned char>(g_)), blue(static_cast<unsigned char>(b_)) {}
            rgb(int rgb) { red = (rgb >> 16) & 0xff; green = (rgb >> 8) & 0xff; blue = (rgb) & 0xff; }
            rgb(const rgb& v) { *this = v; }
            // set with normalized values
            rgb(double r_, double g_, double b_) : red(static_cast<unsigned char>(r_*255.)), green(static_cast<unsigned char>(g_*255.)), blue(static_cast<unsigned char>(b_*255.)) {}


            /// get the rgb as a single int value
            uint32_t asInt24() const { return (uint32_t)( red << 16) + (uint32_t)( green << 8) + (uint32_t)blue ; }
            int asInt() const { return (int)asInt24(); }

            /// 32bit with alpha
            uint32_t asInt32() const { return ((uint32_t)(red) << 24) | ((uint32_t)(green) << 16) | ((uint32_t)(blue) << 8) | 0xff; }

            /// 32bit scaled (0..1) with alpha
            uint32_t asInt32( double sr, double sg, double sb) const { return ((uint32_t)(sr*red) << 24) | ((uint32_t)(sg*green) << 16) | ((uint32_t)(sb*blue) << 8) | 0xff; }

            /// is value zero / black
            bool isZero() const { return red == 0 && green == 0 && blue == 0; }

            /// check for max
            bool isWhite() const { return red == 255 && green == 255 && blue == 255; }

            // exchange red <-> blue, keep green
            rgb& swap() { const auto r = red; red = blue; blue = r; return *this; }

            rgb& operator *=(const rgb& pt) { 
                red = red * pt.red / 255; 
                green = green * pt.green / 255;
                blue = blue * pt.blue / 255;
                return *this;
            }
            
            rgb operator *(const rgb& pt) const {
                rgb col;
                col.red = red * pt.red / 255;
                col.green = green * pt.green / 255;
                col.blue = blue * pt.blue / 255;
                return col;
            }

            bool operator == (const rgb& p) const { return red==p.red && green==p.green && blue==p.blue; }
            bool operator != (const rgb& p) const { return red != p.red || green != p.green || blue != p.blue; }

            // get
            unsigned char r() const { return red; }
            unsigned char g() const { return green; }
            unsigned char b() const { return blue; }

            // reference for set
            unsigned char& r() { return red; }
            unsigned char& g() { return green; }
            unsigned char& b() { return blue; }

            // set to zero (black)
            void setZero() { red = green = blue = 0; }


            // set same for red,green and blue (gray value)
            void setGray(unsigned int v) { red = green = blue = v < 255 ? static_cast<unsigned char>(v) : 255; }
            void setGray(int v) { int c = v; if (c < 0) c = 0; else if (c > 255) c = 255; red = green = blue = c; }
            void setGray(unsigned char v) { red = green = blue = v; }

            /// set r, g and b
            void setRGB(unsigned char r_, unsigned char g_, unsigned char b_) { red = r_; green = g_; blue = b_;}

            void setRGB(const rgb& v) { red = v.red; green = v.green; blue = v.blue; }

            /// set via 24bit int value
            rgb& set24(uint32_t v) { red = (v >> 16) & 0xff; green = (v >> 8) & 0xff; blue = v & 0xff; return *this; }
            /// set via 32bit int value, ignor least 8 bits (alpha)
            rgb& set32(uint32_t v) { red = (v >> 24) & 0xff; green = (v >> 16) & 0xff; blue = (v >> 8) & 0xff; return *this; }

        protected:
            unsigned char red {};  ///< value for red
            unsigned char green {};  ///< value for green
            unsigned char blue {};  ///< value for blue
        };

        //=====================================================================================================================================

        class rgba : public rgb
        {
        public:

            rgba() : rgb(), alpha(255) {}

            unsigned char getAlpha() const { return alpha; }
            unsigned char a() const { return alpha; }
            unsigned char& a() { return alpha; }

        protected:
            unsigned char alpha;  ///< alpha
        };

#pragma pack()

    }
}
