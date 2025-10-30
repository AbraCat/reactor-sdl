#include "window.hpp"

dr4::MyWindow::MyWindow(dr4::Vec2f size, std::string title)
{
    width = size.x;
    height = size.y;
    this->title = title;
}

void dr4::MyWindow::SetTitle(const std::string &title)
{
    this->title = title;
}

const std::string& dr4::MyWindow::GetTitle() const
{
    return title;
}

dr4::Vec2f dr4::MyWindow::GetSize() const
{
    return Vec2f(width, height);
}

void dr4::MyWindow::Open()
{
    MyWindow* w;
    if (!SDL_CreateWindowAndRenderer("Hello World", width, height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return;
    }
    // setRenderer(renderer);
}

bool dr4::MyWindow::IsOpen() const { return is_open; }

void dr4::MyWindow::Close()
{
    //
}

// void dr4::MyWindow::Clear(const Color &color);

// void dr4::MyWindow::Draw(const Drawable &object);

// void dr4::MyWindow::Draw(const Texture &texture, Vec2f pos);

void dr4::MyWindow::Display()
{
    //
}

// std::optional<Event> PollEvent();