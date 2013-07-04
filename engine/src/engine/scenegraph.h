#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

class QOpenGLShaderProgram;

#include <string>
#include <map>
#include <set>
#include <vector>

namespace Engine {

class ModelInstance;
class Renderable;
class Texture;
class Light;

class SceneGraph
{
public:
    typedef std::set<ModelInstance*> Instances;
    typedef std::map<Renderable*, Instances> RenderableNode;
    typedef std::map<Texture*, RenderableNode> TextureNode;
    typedef std::map<QOpenGLShaderProgram*, TextureNode> ProgramNode;

    void addInstance(ModelInstance* instance);
    bool removeInstance(ModelInstance* instance, bool recursive = false);

    void addLight(Light* light);
    bool removeLight(Light* light);

    void clear();

    const ProgramNode& nodes() const;
    const std::vector<Light*> lights() const;

private:
    ProgramNode graph_;
    std::vector<Light*> lights_;
};

}

#endif //SCENEGRAPHH