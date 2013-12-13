template<class T>
void BaseVisitable::acceptVisitor(T& visited, BaseVisitor& visitor)
{
    Visitor<T>* ptr = dynamic_cast<Visitor<T>*>(&visitor);
    if(ptr != nullptr)
    {
        ptr->visit(visited);
    }
}