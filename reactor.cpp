#include "reactor.h"
#include "sdl-adapter.h"

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

Molecule::Molecule(int mass, Vector v, Vector pos, MolType type, Reactor* reactor) : reactor(reactor)
{
    this->status = MOL_VALID;

    this->mass = mass;
    this->r = unitRadius * std::sqrt(mass);
    this->v = v;
    this->pos = pos;
    this->type = type;
}

RoundMol::RoundMol(int mass, Vector v, Vector pos, Reactor* reactor) : 
    Molecule(mass, v, pos, MOL_ROUND, reactor) {};
SquareMol::SquareMol(int mass, Vector v, Vector pos, Reactor* reactor) : 
    Molecule(mass, v, pos, MOL_SQUARE, reactor) {};

void RoundMol::collide(std::vector<Molecule*>& mols, Vector collidePos, Molecule* other)
{
    switch (other->type)
    {
        case MOL_ROUND:
        {
            Vector newV = (this->v * this->mass + other->v * other->mass) / (this->mass + other->mass);
            SquareMol* newMol = new SquareMol(this->mass + other->mass, newV, collidePos, reactor);
            mols.push_back(newMol);
            break;
        }
        case MOL_SQUARE:
            Vector newV = (this->v * this->mass + other->v * other->mass) / (this->mass + other->mass);
            SquareMol* newMol = new SquareMol(this->mass + other->mass, newV, collidePos, reactor);
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
                mols.push_back(new RoundMol(1, newV, collidePos + newV * explodeDT, reactor));
            }

            break;
        }
    }
}

void RoundMol::draw()
{
    reactor->t->addCircle(pos, {0, 0, 255}, r, 1);
}

void SquareMol::draw()
{
    reactor->t->addRect({pos - Vector(r, r), pos + Vector(r, r)}, {255, 0, 0}, 1);
}

Molecule* Reactor::randMolecule()
{
    Vector v = Vector(randDouble(-spawnV, spawnV), randDouble(-spawnV, spawnV), 0);
    Vector pos = Vector(randDouble(spawnPad, width - spawnPad), 
                        randDouble(spawnPad, height - spawnPad), 0);

    if (rand() % 2) return new RoundMol(1, v, pos, this);
    else return new SquareMol(randInt(1, spawnM), v, pos, this);
}

void Reactor::moveWall(int step)
{
    tl.x += step;
    walltl.x += step;
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

Reactor::Reactor(Vector tl, Vector br, Widget* parent) : Widget(tl, br, parent)
{
    resize(tl, br);
    setWidgetBorderVisible(1);

    mols = std::vector<Molecule*>();
    mols.reserve(nReserve);
    for (int nMol = 0; nMol < nSpawn; ++nMol)
        mols.push_back(randMolecule());

    this->lftTemp = 1;
}

Reactor::~Reactor()
{
    for (Molecule* mol: mols)
        delete mol;
}

void Reactor::resize(Vector newtl, Vector newbr)
{
    Widget::resize(newtl, newbr);
    
    walltl = Vector(wallDist, wallDist, 0);
    wallbr = Vector(width, height) - Vector(wallDist, wallDist, 0);
}

void Reactor::checkWallCollision(Molecule* mol)
{
    Vector newPos = mol->pos + mol->v * dt;

    if (newPos.x > wallbr.x)
    {
        // rgtImpulse += mol->mass * mol->v.x;
        mol->pos = Vector(2 * wallbr.x - newPos.x, newPos.y, 0);
        mol->v.x *= -1;
    }
    else if (newPos.x < walltl.x)
    {
        mol->pos = Vector(2 * walltl.x - newPos.x, newPos.y, 0);
        mol->v.x *= -1;
        mol->v.x += lftTemp / mol->mass;
    }
    else if (newPos.y < walltl.y)
    {
        mol->pos = Vector(newPos.x, 2 * walltl.y - newPos.y, 0);
        mol->v.y *= -1;
    }
    else if (newPos.y > wallbr.y)
    {
        mol->pos = Vector(newPos.x, 2 * wallbr.y - newPos.y, 0);
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
    paint();
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
    Widget::paint();
    t->addRect({walltl, wallbr}, {255, 255, 255}, 0);

    for (std::vector<Molecule*>::iterator molIter = mols.begin(); molIter != mols.end(); molIter++)
    {
        Molecule* mol = *molIter;
        mol->draw();
    }
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
