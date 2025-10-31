#ifndef I_ABRACAT_DR4_PLUGIN
#define I_ABRACAT_DR4_PLUGIN

#include "dr4_ifc.hpp"

namespace dr4
{

class AbraCat_DR4Backend : public dr4::DR4Backend
{
public:
    AbraCat_DR4Backend();

    virtual const std::string &Name() const override;
    virtual dr4::Texture *CreateTexture() override;
    virtual dr4::Window *CreateWindow() override;

private:
    std::string name;
};

extern "C" DR4Backend* CreateDR4Backend(void);

}

#endif // I_ABRACAT_DR4_PLUGIN