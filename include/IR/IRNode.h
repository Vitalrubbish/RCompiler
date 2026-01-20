#ifndef IRNODE_H
#define IRNODE_H
class IRVisitor;

class IRNode {
public:
	virtual ~IRNode() = default;

    virtual void accept(IRVisitor *visitor) {}

private:
	virtual void print() {}
};
#endif //IRNODE_H
