#ifndef TYPES_HH
#define TYPES_HH

#include "CppUnitTest.h"

#include <QVector3D>

namespace Test = Microsoft::VisualStudio::CppUnitTestFramework;

template<> static std::wstring Test::ToString<QVector3D>(const QVector3D& v)
{
	std::wstringstream ss;
	ss << L"(" << v.x() << L", " << v.y() << L", " << v.z() << L")";
	return ss.str();
}

#endif