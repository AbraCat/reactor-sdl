#include "reactor.h"

#include <cstdlib>
#include <cmath>
#include <cstdio>

const double Pi = 3.1415926;

const double dt = 1, explodeDT = 0.3, unitRadius = 5, spawnV = 10;
const int spawnM = 1, spawnPad = 10, nSpawn = 300, nReserve = std::max(100, nSpawn * 3);

const int wallDist = 20, buttonSize = 50, buttonGap = 10;

bool isZero(double a)
{
    const double eps = 1e-3;
    return a > -eps && a < eps;
}

int randInt(int lft, int rgt) { return rand() % (rgt - lft + 1) + lft;}
double randDouble(double lft, double rgt) { return rand() / (RAND_MAX / (rgt - lft)) + lft; }

void solveQuadratic(double a, double b, double c, double* x1, double* x2, int* nRoots)
{
    double d = b * b - 4 * a * c;

    if (isZero(d))
    {
        *nRoots = 1;
        *x1 = -b / (2 * a);
        *x2 = 0;
        return;
    }

    if (d < 0)
    {
        *nRoots = 0;
        *x1 = *x2 = 0;
        return;
    }

    *nRoots = 2;
    *x1 = (-b - std::sqrt(d)) / (2 * a);
    *x2 = (-b + std::sqrt(d)) / (2 * a);
}

Molecule::Molecule(int mass, Vector v, Vector pos, MolType type)
{
    this->status = MOL_VALID;

    this->mass = mass;
    this->r = unitRadius * std::sqrt(mass);
    this->v = v;
    this->pos = pos;
    this->type = type;
}

RoundMol::RoundMol(int mass, Vector v, Vector pos) : Molecule(mass, v, pos, MOL_ROUND) {};
SquareMol::SquareMol(int mass, Vector v, Vector pos) : Molecule(mass, v, pos, MOL_SQUARE) {};

void RoundMol::collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other)
{
    switch (other->type)
    {
        case MOL_ROUND:
        {
            Vector newV = (this->v * this->mass + other->v * other->mass) / (this->mass + other->mass);
            SquareMol* newMol = new SquareMol(this->mass + other->mass, newV, collidePos);
            mols.push_back(newMol);
            break;
        }
        case MOL_SQUARE:
            Vector newV = (this->v * this->mass + other->v * other->mass) / (this->mass + other->mass);
            SquareMol* newMol = new SquareMol(this->mass + other->mass, newV, collidePos);
            mols.push_back(newMol);
            break;
    }
}

void SquareMol::collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other)
{
    switch (other->type)
    {
        case MOL_ROUND:
        {
            other->collide(mols, pos, this);
            break;
        }
        case MOL_SQUARE:
        {
            int n = this->mass + other->mass;
            std::vector<Molecule*> newMols(n);
            double angle0 = randDouble(0, 2 * Pi);
            double vMod = randDouble(1, spawnV);
            Vector vImpulse = (this->v * this->mass + other->v * other->mass) / n;

            for (int i = 0; i < n; ++i)
            {
                double angle = angle0 + i * (2 * Pi / n);
                Vector newV = Vector(vMod * std::cos(angle), vMod * std::sin(angle), 0) + vImpulse;
                mols.push_back(new RoundMol(1, newV, collidePos + newV * explodeDT));
            }

            break;
        }
    }
}

void RoundMol::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_FRect rect;
    rect.x = pos.x - r;
    rect.y = pos.y - r;
    rect.w = rect.h = r * 2;
    SDL_RenderFillRect(renderer, &rect);
}

void SquareMol::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_FRect rect;
    rect.x = pos.x - r;
    rect.y = pos.y - r;
    rect.w = rect.h = r * 2;
    SDL_RenderFillRect(renderer, &rect);
}

Molecule* Reactor::randMolecule()
{
    Vector v = Vector(randDouble(-spawnV, spawnV), randDouble(-spawnV, spawnV), 0);
    Vector pos = Vector(randDouble(TL.x + spawnPad, BR.x - spawnPad), 
                        randDouble(TL.y + spawnPad, BR.y - spawnPad), 0);

    if (rand() % 2) return new RoundMol(1, v, pos);
    else return new SquareMol(randInt(1, spawnM), v, pos);
}

void Reactor::moveWall(int step)
{
    TL.x += step;
    wallTL.x += step;
}

void Reactor::increaseTemp(double step)
{
    lftTemp += step;
}

void Reactor::addRandomMols(int nMols)
{
    if (nMols >= 0)
    {
        while (nMols--)
        {
            mols.push_back(randMolecule());
        }
        return;
    }

    nMols *= -1;
    while (nMols--)
    {
        int randIndex = rand() % mols.size();
        mols[randIndex]->status = MOL_INVALID;
    }
}

void Reactor::addButton(Vector color)
{
    // int nButton = buttons.size();
    // buttons.push_back(new Button(TL.x + (buttonSize + buttonGap) * nButton, TL.y + buttonSize + 10,
    //                              TL.x + (buttonSize) * (nButton + 1) + buttonGap * nButton, TL.y + 10, color));

    //
}

Reactor::Reactor(SDL_Renderer* renderer, IntVec TL, IntVec BR) : Widget(renderer, TL, BR)
{
    #define BUTTON_ACTION(function)\
    QObject::connect(buttons[buttons.size() - 1], &Button::pressed, this, [this]{ function; });

    wallTL = TL + IntVec(wallDist, wallDist, 0);
    wallBR = BR - IntVec(wallDist, wallDist, 0);

    mols = std::vector<Molecule*>();
    mols.reserve(nReserve);
    for (int nMol = 0; nMol < nSpawn; ++nMol)
        mols.push_back(randMolecule());

    // buttons = std::vector<Button*>();
    // addButton(Vector(0, 0, 1));
    // BUTTON_ACTION(moveWall(10))
    // addButton(Vector(0.6, 0, 1));
    // BUTTON_ACTION(moveWall(-10))

    // addButton(Vector(1, 0, 0));
    // BUTTON_ACTION(increaseTemp(1))
    // addButton(Vector(1, 0.6, 0));
    // BUTTON_ACTION(increaseTemp(-1))

    // addButton(Vector(0, 1, 0));
    // BUTTON_ACTION(addRandomMols(10))
    // addButton(Vector(0, 1, 1));
    // BUTTON_ACTION(addRandomMols(-10))

    this->lftTemp = 1;
    #undef BUTTON_ACTION
}

Reactor::~Reactor()
{
    for (Molecule* mol: mols)
        delete mol;
}

void Reactor::resize(IntVec newTL, IntVec newBR)
{
    Widget::resize(newTL, newBR);
    wallTL = TL + IntVec(wallDist, wallDist, 0);
    wallBR = BR - IntVec(wallDist, wallDist, 0);
}

void Reactor::checkWallCollision(Molecule* mol)
{
    Vector newPos = mol->pos + mol->v * dt;

    if (newPos.x > wallBR.x)
    {
        // rgtImpulse += mol->mass * mol->v.x;
        mol->pos = Vector(2 * wallBR.x - newPos.x, newPos.y, 0);
        mol->v.x *= -1;
    }
    else if (newPos.x < wallTL.x)
    {
        mol->pos = Vector(2 * wallTL.x - newPos.x, newPos.y, 0);
        mol->v.x *= -1;
        mol->v.x += lftTemp / mol->mass;
    }
    else if (newPos.y < wallTL.y)
    {
        mol->pos = Vector(newPos.x, 2 * wallTL.y - newPos.y, 0);
        mol->v.y *= -1;
    }
    else if (newPos.y > wallBR.y)
    {
        mol->pos = Vector(newPos.x, 2 * wallBR.y - newPos.y, 0);
        mol->v.y *= -1;
    }
    else
    {
        mol->status = MOL_VALID; // may be removed
        return;
    }

    mol->status = MOL_WALL_BOUNCE;
}

void Reactor::checkMolCollision(Molecule* mol, Molecule* mol2)
{
    Vector V = mol->v - mol2->v, P = mol->pos - mol2->pos;
    double R = mol->r + mol2->r, t1 = 0, t2 = 0;
    int nRoots = 0;

    solveQuadratic(V.x * V.x + V.y * V.y, 2 * (P.x * V.x + P.y * V.y), P.x * P.x + P.y * P.y - R * R,
                   &t1, &t2, &nRoots);
    if (nRoots != 2 || t1 < 0 || t1 > dt) return;

    mol->status = mol2->status = MOL_INVALID;
    Vector critPos1 = mol->pos + mol->v * t1, critPos2 = mol2->pos + mol2->v * t1;
    Vector collidePos = (critPos1 * mol2->r + critPos2 * mol->r) / (mol->r + mol2->r);
    mol->collide(mols, collidePos, mol2);
}

void clearInvalidMols(std::vector<Molecule*>& mols)
{
    for (std::vector<Molecule*>::iterator molIter = mols.begin(); molIter != mols.end(); molIter++)
    {
        switch ((*molIter)->status)
        {
        case MOL_VALID:
            break;
        case MOL_INVALID:
            delete *molIter;
            mols.erase(molIter);
            --molIter;
            break;
        case MOL_WALL_BOUNCE:
            (*molIter)->status = MOL_VALID;
            break;
        }
    }
}

bool Reactor::onIdle(IdleEvent* e)
{
    advance();
    return 0;
}

void Reactor::advance()
{
    std::vector<Molecule*>::iterator molsEnd = mols.end();
    for (std::vector<Molecule*>::iterator molIter = mols.begin(); molIter != molsEnd; molIter++)
    {
        Molecule* mol = *molIter;
        if (mol->status != MOL_VALID) continue;

        checkWallCollision(mol);
        if (mol->status == MOL_WALL_BOUNCE) continue;

        for (std::vector<Molecule*>::iterator molIter2 = mols.begin(); molIter2 != molsEnd; molIter2++)
        {
            if (molIter == molIter2) continue;
            Molecule* mol2 = *molIter2;
            if (mol2->status != MOL_VALID) continue;

            checkMolCollision(mol, mol2);
            if (mol->status == MOL_INVALID) break;
        }

        if (mol->status == MOL_VALID) mol->pos = mol->pos + mol->v * dt;;
    }

    clearInvalidMols(mols);
}

void Reactor::paint()
{
    drawRect(1);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_FRect rect;
    rect.x = wallTL.x;
    rect.y = wallTL.y;
    rect.w = wallBR.x - wallTL.x;
    rect.h = wallBR.y - wallTL.y;
    SDL_RenderRect(renderer, &rect);

    for (std::vector<Molecule*>::iterator molIter = mols.begin(); molIter != mols.end(); molIter++)
    {
        Molecule* mol = *molIter;
        mol->draw(renderer);
    }

    // for (Button* button: buttons)
    // {
    //     Vector color;
    //     if (button->is_pressed) color = button->press_color;
    //     else color = button->unpress_color;
    //     color *= 255;

    //     painter->setBrush(QColor(color.x, color.y, color.z));
    //     painter->drawRect(button->TL.x, -button->TL.y, button->BR.x - button->TL.x, button->TL.y - button->BR.y);
    // }
}

double Reactor::energy()
{
    double ans = 0;
    for (Molecule* mol: mols)
        ans += mol->mass * std::pow(*(mol->v), 2) / 2;
    return ans;
}

std::vector<double> Reactor::molCnt()
{
    int round = 0, square = 0;
    for (Molecule* mol: mols)
    {
        if (mol->type == MOL_ROUND) ++round;
        else ++square;
    }
    return {double(round), double(square)};
}
