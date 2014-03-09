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

using namespace Engine;

GameOfLife::GameOfLife(Engine::ResourceDespatcher& despatcher)
    : FreeLookScene(despatcher), generationNum_(0)
{
    qsrand(QTime::currentTime().msec());
}

GameOfLife::~GameOfLife()
{
}

void GameOfLife::update(unsigned int elapsed)
{
    FreeLookScene::update(elapsed);

    if(input()->keyDown(Qt::Key::Key_F))
    {
        input()->setKey(Qt::Key::Key_F, false);

        qDebug() << "Generation:" << ++generationNum_;
        nextGeneration();
    }

    if(input()->keyDown(Qt::Key::Key_G))
    {
        input()->setKey(Qt::Key::Key_G, false);

        generationNum_ = 0;
        qDebug() << "Generation: 0";
        randomSeed();
    }
}

void GameOfLife::initialise()
{
    // Load skybox
    setSkyboxTexture("assets/skybox/space/space*.png");

    // Set up directional light
    Graph::Light::Ptr dirLight = createLight(Graph::Light::LIGHT_DIRECTIONAL);
    dirLight->setColor(QVector3D(255, 214, 170) / 255.0f);
    dirLight->setDirection(QVector3D(0.0f, -1.0f, -1.0f));
    dirLight->setAmbientIntensity(0.05f);
    dirLight->setDiffuseIntensity(1.0f);

    Renderable::Cube::Ptr cube = Renderable::Primitive<Renderable::Cube>::instance();

    // Initialise space
    for(int x = 0; x < WIDTH; ++x)
    {
        for(int y = 0; y < HEIGHT; ++y)
        {
            // Set fixed transformation
            Population& pop = space_[y][x];
            pop.node = rootNode().createChild();
            pop.node->setScale(0.5f);
            pop.node->setPosition(QVector3D(x, y, 0.5f - (qrand() % 10) / 10.0f));

            // Apply random color
            Material::Ptr material = std::make_shared<Material>();
            material->setDiffuseColor(QVector3D(qrand() % 225 + 25, qrand() % 225 + 25, qrand() % 225 + 25) / 255.0f);
            material->setShininess(50.0f);
            material->setSpecularIntensity(0.7f);

            pop.leaf = std::make_shared<Graph::Geometry>(cube, material);
            scene().addSceneLeaf(pop.leaf);
        }
    }

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

            else if(currentPopulation && neighbours < 4 && neighbours > 1)
            {
                nextPopulation = true;
            }

            nextGen_[y][x] = nextPopulation;
            Population& pop = space_[y][x];

            if(nextPopulation)
            {
                pop.leaf->attach(pop.node);
            }

            else
            {
                pop.leaf->detach();
            }
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
            currentGen_[y][x] = qrand() % modulus == 0;
            Population& pop = space_[y][x];

            if(currentGen_[y][x])
            {
               pop.leaf->attach(pop.node);
            }

            else
            {
                pop.leaf->detach();
            }
        }
    }
}

unsigned int GameOfLife::countNeighbours(const GenerationType& gen, const QPoint& loc)
{
    unsigned int count = 0;

    for(int x = -1; x < 1; ++x)
    {
        for(int y = -1; y < 1; ++y)
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