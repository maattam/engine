// A Sampler function which calculates the average luma from input texture and uses it as
// input for exposure calculation.

#ifndef LUMAEXPOSURE_H
#define LUMAEXPOSURE_H

#include "samplerfunction.h"

#include <QList>
#include <QVector3D>

namespace Engine { namespace Effect {

class LumaExposure : public SamplerFunction<float>
{
public:
    explicit LumaExposure(unsigned int window = 30);
    virtual ~LumaExposure();

    // Sets the input texture.
    // precondition: textureId is valid, width and height correspond to the mipmap level.
    virtual bool setInputTexture(GLint textureId, unsigned int width, unsigned int height, GLuint level);

    // Returns the result of the function.
    // precondition: texture has been set.
    virtual ResultType result();

    // Sets the sample window.
    void setWindowLength(unsigned int window);

private:
    unsigned int window_;
    unsigned int width_;
    unsigned int height_;

    GLuint samplePbo_[2];
    GLuint sampleLevel_;
    int writeIndex_;
    int readIndex_;

    QList<float> exposures_;

    QVector3D sampleTexture();
    float calculateExposure(const QVector3D& linearSample);
};

}}

#endif // LUMAEXPOSURE_H