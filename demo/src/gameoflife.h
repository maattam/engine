//
//  Author   : Matti Määttä
//  Summary  : Conway's Game of Life -scene
//

#ifndef GAMEOFLIFE_H
#define GAMEOFLIFE_H

#include "freelookscene.h"

#include "scene/importednode.h"
#include "graph/geometry.h"

#include <memory>
#include <array>

#include <QPoint>

class GameOfLife : public Engine::Ui::FreeLookScene
{
public:
    enum { WIDTH = 256, HEIGHT = 128 };

    explicit GameOfLife(Engine::ResourceDespatcher& despatcher);
    ~GameOfLife();

    // Reimplemented methods from FreeLookScene
    virtual void update(unsigned int elapsed);

protected:
    // Implemented methods from FreeLookScene
    virtual void initialise();

private:
    unsigned int generationNum_;
    bool autoAdvance_;

    struct Population
    {
        Engine::Graph::SceneNode* node;
        Engine::Graph::Geometry::Ptr leaf;
    };

    typedef std::array<Population, WIDTH> RowType;
    typedef std::array<RowType, HEIGHT> SpaceType;

    SpaceType space_;

    typedef std::array<bool, WIDTH> GenRowType;
    typedef std::array<GenRowType, HEIGHT> GenerationType;

    GenerationType currentGen_;
    GenerationType nextGen_;

    unsigned int countNeighbours(const GenerationType& gen, const QPoint& loc);

    QPoint wrap(QPoint loc);

    // Sets and displays tile
    void setGeneration(GenerationType& gen, int x, int y, bool alive);

    void nextGeneration();
    void randomSeed();
};

#endif // GAMEOFLIFE_H