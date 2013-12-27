// A convenience function which uses the cached uniform name if resolved,
// or resolves and caches to location before setting the value.
// precondition: Technique is enabled
// postcondition: Value set and name cached when true is returned.
template<typename T>
bool Technique::setUniformValue(const QString& name, T&& value)
{
    int location = cachedUniformLocation(name);
    if(location == -1)
    {
        location = resolveUniformLocation(name);
    }

    if(location == -1)
    {
        return false;
    }

    program()->setUniformValue(location, std::forward<T>(value));
    return true;
}