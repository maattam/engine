#include "stdafx.h"
#include "CppUnitTest.h"

#include <QMatrix4x4>
#include <QList>

#include "mathelp.h"
#include "frustum.h"

using namespace Engine;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    TEST_CLASS(frustum)
    {
    public:
        frustum::frustum()
            : unitBox_(QVector3D(-0.5f, -0.5f, -0.5f), QVector3D(0.5f, 0.5f, 0.5f))
        {
            viewProj_.perspective(45.0f, 1.0f, 0.1f, 100.0f);
            viewProj_.lookAt(QVector3D(0, 0, 0), UNIT_Z, UNIT_Y);
        }
        
        TEST_METHOD(InsideFrustum)
        {
            // Move the box in front of the camera
            QMatrix4x4 model;
            model.translate(UNIT_Z * 10.0f);

            Assert::IsTrue(isInsideFrustum(unitBox_, viewProj_ * model));

            // Off-origin camera
            QMatrix4x4 offcam;
            offcam.perspective(45.0f, 1.0f, 0.1f, 100.0f);
            offcam.lookAt(QVector3D(10, 0, 10), QVector3D(10, 0, 10) + UNIT_Z, UNIT_Y);

            model.translate(QVector3D(10, 0, 20));
            Assert::IsTrue(isInsideFrustum(unitBox_, viewProj_ * model));
        }

        TEST_METHOD(OutsideFrustum)
        {
            const QList<QVector3D> TEST_POINTS{
                QVector3D(0.0f,     0.0f,       -50.0f),	// Move the box behind the camera
                QVector3D(-50.0f,   0.0f,       5.0f),		// Move the box to left
                QVector3D(50.0f,    0.0f,       5.0f),		// Move the box to right
                QVector3D(0.0f,	    0.0f,       105.0f),	// Move the box outside far
                QVector3D(0.0f,     50.0f,      5.0f),		// Move the box upwards
                QVector3D(0.0f,     -50.0f,     5.0f)		// Move the box downwards
            };

            for(const QVector3D& point : TEST_POINTS)
            {
                QMatrix4x4 model;
                model.translate(point);
                Assert::IsFalse(isInsideFrustum(unitBox_, viewProj_ * model), ToString(point).c_str());
            }
        }

        TEST_METHOD(FrustumInside)
        {
            AABB aabb(QVector3D(1, 1, 1) * -500, QVector3D(1, 1, 1) * 500);
            Assert::IsTrue(isInsideFrustum(aabb, viewProj_), L"500box");

            const float inf = std::numeric_limits<float>::max() / 2.0f;
            AABB infAABB(QVector3D(1, 1, 1) * -inf, QVector3D(1, 1, 1) * inf);
            Assert::IsTrue(isInsideFrustum(infAABB, viewProj_), L"infbox");
        }

    private:
        QMatrix4x4 viewProj_;
        AABB unitBox_;

    };
}