#ifdef _MSC_VER
    // decorated name length exceeded, name was truncated
    #pragma warning(disable : 4503)
#endif

#include "scenegraph.h"

#include <cassert>

#include "modelinstance.h"

using namespace Engine;

void SceneGraph::addInstance(ModelInstance* instance)
{
    // Sanity checks, we allow null textures though
    if(instance == nullptr || instance->program() == nullptr || instance->renderable() == nullptr)
        return;

    // Find program node
    ProgramNode::iterator programNode = graph_.find(instance->program());

    // No program found, add all nodes to graph
    if(programNode == graph_.end())
    {
        graph_[instance->program()][instance->texture()][instance->renderable()].insert(instance);
    }

    else
    {
        // Find texture node
        TextureNode::iterator textureNode = programNode->second.find(instance->texture());

        // No texture node found, add the rest to graph
        if(textureNode == programNode->second.end())
        {
            programNode->second[instance->texture()][instance->renderable()].insert(instance);
        }

        else
        {
            // Find renderable node
            RenderableNode::iterator renderableNode = textureNode->second.find(instance->renderable());

            // No renderable node found, add renderable and instance to graph
            if(renderableNode == textureNode->second.end())
            {
                textureNode->second[instance->renderable()].insert(instance);
            }

            else
            {
                // Renderable found, add instance (set makes sure there are no duplicates)
                renderableNode->second.insert(instance);
            }
        }
    }
}

bool SceneGraph::removeInstance(ModelInstance* instance, bool recursive)
{
    if(instance == nullptr)
        return false;

    ProgramNode::iterator programNode = graph_.find(instance->program());
    if(programNode != graph_.end())
    {
        TextureNode::iterator textureNode = programNode->second.find(instance->texture());
        if(textureNode != programNode->second.end())
        {
            RenderableNode::iterator renderableNode = textureNode->second.find(instance->renderable());
            if(renderableNode != textureNode->second.end())
            {
                if(renderableNode->second.erase(instance) > 0)
                {
                    // If renderable node is empty, we can delete it
                    if(recursive && renderableNode->second.empty())
                    {
                        textureNode->second.erase(renderableNode);

                        // If texture node is empty, we can delete it too
                        if(textureNode->second.empty())
                        {
                            programNode->second.erase(textureNode);

                            // If the whole program node is empty, we can discard it
                            if(programNode->second.empty())
                            {
                                graph_.erase(programNode);
                            }
                        }
                    }

                    return true;
                }
            }
        }
    }

    return false;
}

void SceneGraph::clear()
{
    graph_.clear();
    lights_.clear();
}

const SceneGraph::ProgramNode& SceneGraph::nodes() const
{
    return graph_;
}

const std::vector<Light*> SceneGraph::lights() const
{
    return lights_;
}

void SceneGraph::addLight(Light* light)
{
    lights_.push_back(light);
}

bool SceneGraph::removeLight(Light* light)
{
    for(auto it = lights_.begin(); it != lights_.end(); ++it)
    {
        if(*it == light)
        {
            lights_.erase(it);
            return true;
        }
    }

    return false;
}