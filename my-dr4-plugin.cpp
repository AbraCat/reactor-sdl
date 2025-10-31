#include "my-dr4-plugin.h"
#include "window.hpp"

const int window_w = 1920, window_h = 1000, texture_size = 300;

extern "C" dr4::DR4Backend* CreateDR4Backend(void)
{
    return new dr4::AbraCat_DR4Backend();
}

dr4::AbraCat_DR4Backend::AbraCat_DR4Backend()
{
    name = "AbraCat dr4 plugin";
}

const std::string& dr4::AbraCat_DR4Backend::Name() const
{
    return name;
}

dr4::Texture * dr4::AbraCat_DR4Backend::CreateTexture()
{
    return new MyTexture(dr4::Vec2f(texture_size, texture_size));
}

dr4::Window * dr4::AbraCat_DR4Backend::CreateWindow()
{
    return new MyWindow(dr4::Vec2f(window_w, window_h), "Optical Constructor");
}