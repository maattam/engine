template<typename SceneType>
struct _RegisterScene
{
    static SceneController* create(ResourceDespatcher& despatcher)
    {
        return new SceneType(despatcher);
    }
};

template<typename SceneType>
void SceneFactory::registerScene(const QString& name)
{
    sceneTypes_[name] = _RegisterScene<SceneType>::create;
}