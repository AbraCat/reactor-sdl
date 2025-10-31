#ifndef I_HUI_TEXTURE
#define I_HUI_TEXTURE

#include <string>

#include "color.hpp"
#include "rect.hpp"
#include "vec2.hpp"

#include "SDL3/SDL.h"

namespace dr4
{

    class Texture;

    struct Rectangle
    {
        Rectangle(Rect2f r, Color fill, Color border_col);

        Rect2f rect;
        Color fill;
        float borderThickness;
        Color borderColor;
    };

    // struct Text {

    //     enum class VAlign {
    //         UNKNOWN = -1,
    //         TOP,
    //         MIDDLE,
    //         BASELINE,
    //         BOTTOM
    //     };

    //     const std::string text;
    //     Vec2f pos;
    //     Color color;
    //     float fontSize;
    //     VAlign valign = VAlign::TOP;

    //     virtual Rect2f GetBounds() const = 0;

    //     virtual void DrawOn(Texture &texture) const = 0;
    // };

    class Image
    {
    public:
        virtual void SetPix(int x, int y, dr4::Color col) = 0;
        virtual dr4::Color GetPix(int x, int y) const = 0;
    };

    class MyImage : public Image
    {
        void *buffer;
        unsigned char *buf;
        // PixelFormat format; // RGBA32, ...
        size_t width, height;
        size_t stride; // количество байт на один ряд

    public:
        MyImage(int w, int h);
        ~MyImage();

        virtual void SetPix(int x, int y, dr4::Color col) override;
        virtual dr4::Color GetPix(int x, int y) const override;
    };

    class Texture
    {
    public:
        virtual void SetSize(Vec2f size) = 0;
        virtual Vec2f GetSize() const = 0;
        virtual float Width() const = 0;
        virtual float Height() const = 0;

        virtual void Draw(const Rectangle &rect) = 0;
        virtual void Draw(const Image &img) = 0;
        // virtual void Draw(Texture &texture, const Vec2f &pos) = 0;

        // virtual void Draw(const Image &img) = 0;
        // virtual Image* lock() = 0;
        // virtual void unlock(Image* img) = 0;
    };

    class MyTexture : public Texture
    {
    public:
        MyTexture(dr4::Vec2f size);

        virtual void SetSize(Vec2f size);
        virtual Vec2f GetSize() const;
        virtual float Width() const;
        virtual float Height() const;

        virtual void Draw(const Rectangle &rect);
        // virtual void Draw(Texture &texture, const Vec2f &pos);
        virtual void Draw(const Image &img) override;

        bool lockable;
        int w, h;
        SDL_Texture *t;
    };

}; // namespace hui

#endif // I_HUI_TEXTURE
