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

        TEST_METHOD(IrregularBox)
        {
            const QVector3D MIN = QVector3D(-2.0f, 0.0f, -3.0f);
            const QVector3D MAX = QVector3D(1.0f, 5.0f, 5.0f);

            AABB aabb(MIN, MAX);

            Assert::AreEqual(3.0f, aabb.width());
            Assert::AreEqual(5.0f, aabb.height());
            Assert::AreEqual(8.0f, aabb.depth());

            Assert::AreEqual((MAX + MIN) / 2.0f, aabb.center());
        }

    };
}