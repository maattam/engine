#ifndef NODE_H
#define NODE_H

// Abstract graph node

#include <QMatrix4x4>
#include <vector>

namespace Engine { namespace Graph {

class Node
{
public:
    typedef std::vector<Node*> ChildNodes;

    Node();
    virtual ~Node();

    Node* getParent() const;

    const QMatrix4x4& transformation() const;
    void applyTransformation(const QMatrix4x4& matrix);
    void setPosition(const QVector3D& position);

    ChildNodes::size_type numChildren() const;
    Node* getChild(ChildNodes::size_type index);

    // Does not delete nodes!
    virtual Node* removeChild(ChildNodes::size_type index);
    virtual Node* removeChild(Node* child);
    virtual void removeAllChildren();

    virtual Node* createChild(const QMatrix4x4& transformation = QMatrix4x4());
    virtual void addChild(Node* child);

protected:
    void setParent(Node* parent);

    virtual Node* createChildImpl(const QMatrix4x4& transformation) = 0;

private:
    Node* parent_;
    ChildNodes children_;
    QMatrix4x4 transformation_;
};

}}

#endif //NODE_H