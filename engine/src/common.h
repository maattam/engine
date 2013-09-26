#ifndef COMMON_H
#define COMMON_H

// Qt Opengl version and profile to use
#define QOPENGL_PROFILE 4_2_Core

#define EXPAND(x) x
#define QOPENGL_FUNCTIONS EXPAND(QOpenGLFunctions_)EXPAND(QOPENGL_PROFILE)
#define QOPENGL_HEADER <QOPENGL_FUNCTIONS>

#ifdef _DEBUG
    #define RESOURCE_PATH(x) EXPAND("../../engine/")x
#else
    #define RESOURCE_PATH(x) EXPAND(":/")x
#endif // _DEBUG

#include QOPENGL_HEADER

extern QOPENGL_FUNCTIONS* gl;

#endif // COMMON_H