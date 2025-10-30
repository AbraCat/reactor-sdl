#ifndef I_HUI_TEXTURE
#define I_HUI_TEXTURE

#include <string>

#include "color.hpp"
#include "rect.hpp"
#include "vec2.hpp"

namespace dr4 {

class Texture;

struct Drawable {
    virtual void DrawOn(Texture &texture) = 0;
};

// struct Pixel : public Drawable {

//     Vec2f pos;
//     Color color;

//     // void DrawOn(Texture &texture) { texture.SetPix(pos.x, pos.y, color); }
// };

struct Rectangle : public Drawable {

    Rect2f rect;
    Color fill;
    float borderThickness;
    Color borderColor;

    virtual void DrawOn(Texture &texture);
};

struct Text : public Drawable {

    enum class VAlign {
        UNKNOWN = -1,
        TOP,
        MIDDLE,
        BASELINE,
        BOTTOM
    };

    const std::string text;
    Vec2f pos;
    Color color;
    float fontSize;
    VAlign valign = VAlign::TOP;

    virtual Rect2f GetBounds() const = 0;

    virtual void DrawOn(Texture &texture) const = 0;
};

class Texture {

public:

    Texture(Vec2f size);

    virtual void SetSize(Vec2f size);
    virtual Vec2f GetSize();
    virtual float Width();
    virtual float Height();

    inline void Draw(Drawable &object) { object.DrawOn(*this); }
    virtual void Draw(Texture &texture, const Vec2f &pos);

    inline void SetPix(int x, int y, dr4::Color col) { pix[y * w + x] = col; }
    inline dr4::Color GetPix(int x, int y) { return pix[y * w + x]; }

    int w, h;
    Color* pix;
};

}; // namespace hui

#endif // I_HUI_TEXTURE
