#ifndef REACTOR_H
#define REACTOR_H

#include <SDL3/SDL.h>

#include <vector>

#include "myvector.h"
#include "widget.h"

enum MolType
{
    MOL_ROUND,
    MOL_SQUARE
};

enum MolStatus
{
    MOL_VALID,
    MOL_INVALID,
    MOL_WALL_BOUNCE
};

class Molecule;
class RoundMol;
class SquareMol;
class Reactor;

class Molecule
{
public:
    MolStatus status;
    MolType type;
    int mass, r;
    Vector v, pos;

    Molecule(int mass, Vector v, Vector pos, MolType type);
    virtual ~Molecule() = default;

    virtual void collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other) = 0;
    virtual void draw(SDL_Renderer* renderer) = 0;
};

class RoundMol : public Molecule
{
public:
    RoundMol(int mass, Vector v, Vector pos);

    virtual void collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other);
    virtual void draw(SDL_Renderer* renderer) override;
};

class SquareMol : public Molecule
{
public:
    SquareMol(int mass, Vector v, Vector pos);

    virtual void collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other);
    virtual void draw(SDL_Renderer* renderer) override;
};

class Button : public Widget
{
public:
    Button(SDL_Renderer* renderer, IntVec TL, IntVec BR, Vector color);
    virtual void action();
    void unpress();

    Vector press_color, unpress_color;
    bool is_pressed;
};

// class MoveButton : public Button
// {
//     Q_OBJECT
// public:
//     virtual void action() override;
// };

class Reactor : public Widget
{
public:
    Reactor(SDL_Renderer* renderer, IntVec TL, IntVec BR);
    ~Reactor();
    virtual void paint() override;

    // virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    // virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    Molecule* randMolecule();

    double energy();
    std::vector<double> molCnt();

    void checkWallCollision(Molecule* mol);
    void checkMolCollision(Molecule* mol, Molecule* mol2);

    void moveWall(int step);
    void increaseTemp(double step);
    void addRandomMols(int nMols);

    void addButton(Vector color);
    void advance();

    IntVec wallTL, wallBR;
    std::vector<Molecule*> mols;

    std::vector<Button*> buttons;
    double lftTemp, rgtImpulse;
};

#endif // REACTOR_H