#ifndef IRNODE_H
#define IRNODE_H
class IRNode {
public:
	virtual ~IRNode() = default;

private:
	virtual void print() {}
};
#endif //IRNODE_H
