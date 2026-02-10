#ifndef RCOMPILER_ASMNODE_H
#define RCOMPILER_ASMNODE_H

class ASMNode {
public:
	virtual ~ASMNode() = default;

	virtual void print() = 0;
};

#endif //RCOMPILER_ASMNODE_H