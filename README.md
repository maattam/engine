Engine
======

## Small expeditionary rendering engine using Qt and OpenGL

Engine is a hobby project motivated by my interest in computer graphics, algorithms
and low-level hardware implementation. My intention is not to make a full-fledged game
engine, but instead an engine where I can experiment with different signal processing and
computer graphics related algorithms. 

The project started as an introduction to basic [OpenGL] concepts, but has since grown to feature
forward and modern deferred rendering, post-processing, shadow mapping and different lightning models.

Most of the project is written in C\+\+, but the user interface contains some JavaScript -derivate QML bits.
The project architecture focuses on iterative developement by avoiding dependencies to implementations and
employs heavy use of [SOLID] object-oriented design as a result. The project depends heavily on Qt,
[Qt Quick] and [OpenGL] 4.2.

Author
------
Matti Määttä ([maatta.matti@gmail.com])

Features
--------

* ** Deferred rendering **

    Deferred rendering is two-stage rendering method where the visible geometry is first accumulated to a
    geometry buffer (**GBuffer**) containing normals, depth and material properties. Lightning is applied in the
    second rendering pass as a post-processing effect. This allows drawing hundreds of dynamic lights with a small
    performance impact compared to forward rendering. Multisample anti-aliasing is supported in modern [OpenGL]
    pipeline using centroid-sampling edge detection.

    ![deferred gbuffer](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/gbuffer.jpg)

* ** Qt Quick integration **

    [Qt Quick] allows for easy creation of dynamic user interfaces using JavaScript-based QML declarative language.
    Qt allows easy signaling between QML and C\+\+ parts of the engine. In the future I might look into scripting whole
    scenes in QML instead of C\+\+.

    ![qtquick integration](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/qtquick.jpg)

* ** Asset importing using Assimp **

    [Assimp] converts many well-known 3D model or scene formats to internal Engine scene graph -supported structures.
	This enables easy scene deployment with lights and cameras from eg. [Blender] without writing a single line of code.

Usage
-----
Scenes can be created by deriving new classes from Ui::SceneController and registering them to
Ui::SceneFactory. See example demo/src/main.cpp.

Compiling is only possible at the moment on Visual Studio 2012 and above. See the list of dependencies below.
Unfortunately the included demo can't be run since I can't publish the used assets.

Dependencies
------------
- **Qt 5.2 with OpenGL** [Qt]
- **Assimp** [Assimp]
- **OpenGL Image (GLI)** [GLI]

Screenshots
-----------

### Scene with 120 lights and post-processing

[![scrn sponza1](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/sponza1.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/sponza1.jpg)
[![scrn sponza2](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/sponza2.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/sponza2.jpg)

### Dragon

[![scrn dragon](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/dragon.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/dragon.jpg)

### Scene with 500 point lights

[![scrn 500lights](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/500lights.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/500lights.jpg)

## Forward rendering with shadows

[![scrn forward](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/forward.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/forward.jpg)

## Single-pass edge detection for deferred MSAA

[![scrn edge](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/edgedetect.jpg)](https://bitbucket.org/maattam/engine/raw/ac963b07500b0b510c733c2b0dab75d3bd2b9288/images/edgedetect.jpg)

[OpenGL]: http://www.opengl.org/
[Qt]: http://qt-project.org/
[Qt Quick]: http://qt-project.org/doc/qt-5.0/qtquick/qtquick-index.html
[Assimp]: http://assimp.sourceforge.net/
[Blender]: http://www.blender.org/
[SOLID]: http://en.wikipedia.org/wiki/SOLID_(object-oriented_design)
[GLI]: http://www.g-truc.net/project-0024.html
[maatta.matti@gmail.com]: mailto:maatta.matti@gmail.com