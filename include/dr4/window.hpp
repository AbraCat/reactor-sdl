#ifndef I_HUI_WINDOW
#define I_HUI_WINDOW

#include <string>
#include <optional>

#include "event.hpp"
#include "color.hpp"
#include "vec2.hpp"
#include "texture.hpp"
#include "widget.hpp"

#include "SDL3/SDL.h"

namespace dr4 {

class Window {

    // Texture field

public:

    virtual void SetTitle(const std::string &title) = 0;
    virtual const std::string &GetTitle() const = 0;

    virtual Vec2f GetSize() const = 0;

    virtual void Open() = 0;
    virtual bool IsOpen() const = 0;
    virtual void Close() = 0;

    // virtual void Clear(const Color &color) = 0;

    // virtual void Draw(const Drawable &object) = 0;
    // virtual void Draw(const Texture &texture, Vec2f pos) = 0;

    virtual void Display() = 0;

    // virtual std::optional<Event> PollEvent() = 0;
};

class MyWindow : public Window {
    // hui::Widget* root_widget;
    // dr4::Texture root_texture;

    std::string title;
    int width, height;
    bool is_open;

    SDL_Window *window;
    SDL_Renderer *renderer;

public:
    MyWindow(Vec2f size, std::string title);

    virtual void SetTitle(const std::string &title);
    virtual const std::string &GetTitle() const;

    virtual Vec2f GetSize() const;

    virtual void Open();
    virtual bool IsOpen() const;
    virtual void Close();

    // virtual void Clear(const Color &color);

    // virtual void Draw(const Drawable &object);
    // virtual void Draw(const Texture &texture, Vec2f pos);

    virtual void Display();

    // virtual std::optional<Event> PollEvent();
};

}; // namespace dr4

#endif // I_HUI_WINDOW
