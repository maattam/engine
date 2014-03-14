#include "stdafx.h"
#include "CppUnitTest.h"

#include "aabb.h"

using namespace Engine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    TEST_CLASS(aabb)
    {
    public:
        
        TEST_METHOD(OneByOne)
        {
            AABB aabb(QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(0.5f, 0.5f, 0.5f));

            Assert::AreEqual(1.0f, aabb.width());
            Assert::AreEqual(1.0f, aabb.height());
            Assert::AreEqual(1.0f, aabb.depth());

            Assert::AreEqual(QVector3D(0, 0, 0), aabb.center());

            Assert::IsTrue(aabb.resize(QVector3D(0.0f, 10.0f, 0.0f)));
            Assert::AreEqual(10.5f, aabb.height());
        }

    };
}