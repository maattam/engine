//
//  Author   : Matti Määttä
//  Summary  : 
//

#include "gameoflife.h"

#include "resourcedespatcher.h"
#include "graph/scenenode.h"
#include "graph/light.h"
#include "scene/scenemanager.h"
#include "renderable/primitive.h"
#include "renderable/cube.h"
#include "inputstate.h"
#include "texture2dresource.h"

#include <QTime>
#include <QDebug>
#include <qmath.h>

using namespace Engine;

GameOfLife::GameOfLife(Engine::ResourceDespatcher& despatcher)
    : FreeLookScene(despatcher), generationNum_(0), autoAdvance_(false), totalElapsed_(0)
{
    qsrand(QTime::currentTime().msec());
}

GameOfLife::~GameOfLife()
{
}

void GameOfLife::update(unsigned int elapsed)
{
    FreeLookScene::update(elapsed);
    totalElapsed_ += elapsed;

    if(input()->keyDown(Qt::Key::Key_R))
    {
        input()->setKey(Qt::Key::Key_R, false);
        autoAdvance_ = !autoAdvance_;
    }

    if(input()->keyDown(Qt::Key::Key_F) || autoAdvance_)
    {
        input()->setKey(Qt::Key::Key_F, false);

        if(totalElapsed_ >= 100 || !autoAdvance_)
        {
            totalElapsed_ = 0;
            qDebug() << "Generation:" << ++generationNum_;
            nextGeneration();
        }
    }

    if(input()->keyDown(Qt::Key::Key_G))
    {
        input()->setKey(Qt::Key::Key_G, false);

        generationNum_ = 0;
        qDebug() << "Generation: 0";
        randomSeed();
    }

    rootNode().rotate(M_PI_4 * static_cast<float>(elapsed) / 1000, UNIT_Y);
}

void GameOfLife::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/space/space*.png");

    // Set up directional light
    Graph::Light::Ptr dirLight = createLight(Graph::Light::LIGHT_DIRECTIONAL);
    dirLight->setColor(QVector3D(255, 214, 170) / 255.0f);
    dirLight->setDirection(QVector3D(0.0f, -1.0f, -1.0f));
    dirLight->setAmbientIntensity(0.5f);
    dirLight->setDiffuseIntensity(1.0f);

    Renderable::Cube::Ptr cube = Renderable::Primitive<Renderable::Cube>::instance();

    const float gap = 0.15f;
    const float circ = (WIDTH - 1) * (1 + gap);
    const qreal radius = circ / (2 * M_PI);

    // Initialise space
    for(int x = 0; x < WIDTH; ++x)
    {
        qreal coeff = (x * (1 + gap)) / circ * 2 * M_PI;
        float xcoord = qSin(coeff) * radius;
        float zcoord = qCos(coeff) * radius;

        QVector3D direction = -QVector3D(xcoord, 0, zcoord).normalized();

        for(int y = 0; y < HEIGHT; ++y)
        {
            // Set fixed transformation
            Population& pop = space_[y][x];
            pop.node = rootNode().createChild();
            pop.node->scale(0.5f);
            pop.node->setPosition(QVector3D(xcoord, y * (1 + gap), zcoord) + direction * (0.5f - (qrand() % 10) / 10.0f));
            pop.node->setDirection(direction);

            // Apply random color
            Material::Ptr material = std::make_shared<Material>();
            material->setDiffuseColor(QVector3D(qrand() % 225 + 25, qrand() % 225 + 25, qrand() % 225 + 25) / 255.0f);
            material->setShininess(50.0f);
            material->setSpecularIntensity(0.7f);

            pop.leaf = std::make_shared<Graph::Geometry>(cube, material);
            scene().addSceneLeaf(pop.leaf);
        }
    }

	camera()->setPosition(QVector3D(0, 75, 0));

    randomSeed();
    setFlySpeed(100.0f);
}

void GameOfLife::nextGeneration()
{
    for(int x = 0; x < WIDTH; ++x)
    {
        for(int y = 0; y < HEIGHT; ++y)
        {
            unsigned int neighbours = countNeighbours(currentGen_, QPoint(x, y));
            bool nextPopulation = false;
            bool currentPopulation = currentGen_[y][x];

            if(!currentPopulation && neighbours == 3)
            {
                nextPopulation = true;
            }

            else if(currentPopulation && (neighbours == 2 || neighbours == 3))
            {
                nextPopulation = true;
            }

            setGeneration(nextGen_, x, y, nextPopulation);
        }
    }

    currentGen_ = nextGen_;
}

void GameOfLife::randomSeed()
{
    unsigned int modulus = qrand() % 15 + 5;

    for(int x = 0; x < WIDTH; ++x)
    {
        for(int y = 0; y < HEIGHT; ++y)
        {
            // Make random population
            setGeneration(currentGen_, x, y, qrand() % modulus == 0);
            //setGeneration(currentGen_, x, y, true);
        }
    }
}

void GameOfLife::setGeneration(GenerationType& gen, int x, int y, bool alive)
{
    Population& pop = space_[y][x];
    gen[y][x] = alive;

    if(alive)
    {
        pop.leaf->attach(pop.node);
    }

    else
    {
        pop.leaf->detach();
    }
}

unsigned int GameOfLife::countNeighbours(const GenerationType& gen, const QPoint& loc)
{
    unsigned int count = 0;

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            if(x == 0 && y == 0)
            {
                continue;
            }

            QPoint coord = wrap(loc + QPoint(x, y));
            if(gen[coord.y()][coord.x()])
            {
                count++;
            }
        }
    }

    return count;
}

QPoint GameOfLife::wrap(QPoint loc)
{
    if(loc.x() < 0)
    {
        loc.setX(loc.x() + WIDTH);
    }

    if(loc.y() < 0)
    {
        loc.setY(loc.y() + HEIGHT);
    }

    return QPoint(loc.x() % WIDTH, loc.y() % HEIGHT);
}