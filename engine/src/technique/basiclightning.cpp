#include "basiclightning.h"

#include <QDebug>

#include <sstream>

const double PI = 3.14159;

using namespace Engine;

BasicLightning::BasicLightning() : Technique()
{
}

BasicLightning::~BasicLightning()
{
}

bool BasicLightning::init()
{
    // If our program is already compiled, do nothing
    if(program_.isLinked())
    {
        return true;
    }

    // Load our program
    program_.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/basiclightning.vert");
    program_.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/basiclightning.frag");

    if(!program_.link())
    {
        return false;
    }

    mvpLocation_ = program_.uniformLocation("gMVP");

    return true;
}

void BasicLightning::setMVP(const QMatrix4x4& mvp)
{
    program_.setUniformValue(mvpLocation_, mvp);
}

void BasicLightning::setWorldView(const QMatrix4x4& vp)
{
    program_.setUniformValue("gWorld", vp);
}

void BasicLightning::setEyeWorldPos(const QVector3D& eyePos)
{
    program_.setUniformValue("gEyeWorldPos", eyePos);
}

void BasicLightning::setHasTangents(bool tangents)
{
    program_.setUniformValue("gHasTangents", tangents);
}

void BasicLightning::setMaterialAttributes(const Material::Attributes& attributes)
{
    program_.setUniformValue("gMaterial.ambientColor", attributes.ambientColor);
    program_.setUniformValue("gMaterial.diffuseColor", attributes.diffuseColor);
    program_.setUniformValue("gMaterial.specularIntensity", attributes.specularIntensity);
    program_.setUniformValue("gMaterial.shininess", attributes.shininess);
}

void BasicLightning::setTextureUnits(GLuint diffuse, GLuint normal, GLuint specular)
{
    program_.setUniformValue("gDiffuseSampler", diffuse);
    program_.setUniformValue("gNormalSampler", normal);
    program_.setUniformValue("gSpecularSampler", specular);
}

void BasicLightning::setDirectionalLight(const DirectionalLight& light)
{
    program_.setUniformValue("gDirectionalLight.base.ambientIntensity", light.ambientIntensity);
    program_.setUniformValue("gDirectionalLight.base.color", light.color);
    program_.setUniformValue("gDirectionalLight.base.diffuseIntensity", light.diffuseIntensity);

    QVector3D direction = light.direction;
    direction.normalize();
    program_.setUniformValue("gDirectionalLight.direction", direction);
}

void BasicLightning::setLightMVP(const QMatrix4x4& mvp)
{
    program_.setUniformValue("gLightMVP", mvp);
}

void BasicLightning::setShadowMapTextureUnit(GLuint shadow)
{
    program_.setUniformValue("gShadowMap", shadow);
}

void BasicLightning::setPointLights(const std::vector<PointLight>& lights)
{
    unsigned int numLights = lights.size();
    if(numLights > MAX_POINT_LIGHTS)
    {
        qDebug() << "Warning: MAX_POINT_LIGHTS exceeded";
        numLights = MAX_POINT_LIGHTS;
    }

    program_.setUniformValue("gNumPointLights", numLights);

    for(size_t i = 0; i < numLights; ++i)
    {
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "base.ambientIntensity").c_str(),  lights[i].ambientIntensity);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "base.color").c_str(),             lights[i].color);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "base.diffuseIntensity").c_str(),  lights[i].diffuseIntensity);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "position").c_str(),               lights[i].position);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.constant").c_str(),   lights[i].attenuation.constant);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.exp").c_str(),        lights[i].attenuation.exp);
        program_.setUniformValue(formatUniformTableName("gPointLights", i, "attenuation.linear").c_str(),     lights[i].attenuation.linear);
    }
}

void BasicLightning::setSpotLights(const std::vector<SpotLight>& lights)
{
    unsigned int numLights = lights.size();
    if(numLights > MAX_SPOT_LIGHTS)
    {
        qDebug() << "Warning: MAX_SPOT_LIGHTS exceeded";
        numLights = MAX_SPOT_LIGHTS;
    }

    program_.setUniformValue("gNumSpotLights", numLights);

    for(size_t i = 0; i < numLights; ++i)
    {
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.ambientIntensity").c_str(),  lights[i].ambientIntensity);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.color").c_str(),             lights[i].color);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.base.diffuseIntensity").c_str(),  lights[i].diffuseIntensity);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.position").c_str(),               lights[i].position);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.constant").c_str(),   lights[i].attenuation.constant);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.exp").c_str(),        lights[i].attenuation.exp);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "base.attenuation.linear").c_str(),     lights[i].attenuation.linear);

        QVector3D direction = lights[i].direction;
        direction.normalize();

        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "direction").c_str(),     direction);
        program_.setUniformValue(formatUniformTableName("gSpotLights", i, "cutoff").c_str(),        cosf(PI * lights[i].cutoff / 180));
    }
}

std::string BasicLightning::formatUniformTableName(const std::string& table,
        unsigned int index, const std::string& members) const
{
    std::stringstream ss;
    ss << table << '[' << index << ']' << '.' << members;
    return ss.str();
}