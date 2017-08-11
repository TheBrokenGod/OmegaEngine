#include "GraphicNode.h"

namespace omega {

GraphicNode::GraphicNode(mat4p matrix) :
	parent(nullptr) {
}

GraphicNode::~GraphicNode() {
	for(auto child : children) {
		delete child;
	}
}

void GraphicNode::render() {
}

void GraphicNode::attach(GraphicNode *child) {
	children.insert(child);
}

void GraphicNode::detachFromParent() {
	parent->children.erase(this);
	parent = nullptr;
}

void GraphicNode::forEach(std::function<void(GraphicNode*, mat4p, int)> callback, mat4p worldMatrix, int depth)
{
	// Add local transform to upper global
	mat4 temp = worldMatrix * this->matrix;
	callback(this, temp, depth);
	for (auto child : children)	{
		child->forEach(callback, temp, depth + 1);
	}
}

void GraphicNode::forEach(std::function<void(GraphicNode*, mat4p, int)> callback) {
	forEach(callback, mat4(1), 0);
}

}