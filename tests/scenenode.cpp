#include "stdafx.h"
#include "CppUnitTest.h"

#include "graph/scenenode.h"

using namespace Engine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    TEST_CLASS(scenenode)
    {
    public:
        
        TEST_METHOD(Propagate)
        {
            Graph::SceneNode root;
            root.move(QVector3D(0, 10.0f, 0));
            root.propagate();

            Assert::AreEqual(QVector3D(0, 10.0f, 0), root.worldPosition());

            Graph::SceneNode* child = root.createChild();
            child->move(QVector3D(0, 10.0f, 0));
            root.propagate();

            Assert::AreEqual(QVector3D(0, 20.0f, 0), child->worldPosition());

            Graph::SceneNode* grandChild = child->createChild();
            grandChild->move(QVector3D(0, -10.0f, 0.0f));
            root.propagate();

            Assert::AreEqual(QVector3D(0, 10.0f, 0), grandChild->worldPosition());

            root.move(QVector3D(0, 10.0f, 0));
            root.propagate();

            Assert::AreEqual(QVector3D(0, 20.0f, 0), root.worldPosition());
            Assert::AreEqual(QVector3D(0, 30.0f, 0), child->worldPosition());
            Assert::AreEqual(QVector3D(0, 20.0f, 0), grandChild->worldPosition());
        }

    };
}