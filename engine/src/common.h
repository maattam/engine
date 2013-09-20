#ifndef COMMON_H
#define COMMON_H

// Qt Opengl version and profile to use
#define QOPENGL_PROFILE 4_2_Core

#define EXPAND(x) x
#define QOPENGL_FUNCTIONS EXPAND(QOpenGLFunctions_)EXPAND(QOPENGL_PROFILE)
#define QOPENGL_HEADER <QOPENGL_FUNCTIONS>

#include QOPENGL_HEADER

#endif // COMMON_H