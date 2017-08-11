#pragma once

#include "GraphicObject.h"
#include <set>
#include <functional>

namespace omega {

class GraphicNode : public GraphicObject
{
private:
	GraphicNode* parent;
	std::set<GraphicNode*> children;
	void forEach(std::function<void(GraphicNode*, mat4p, int)> callback, mat4p matrix, int depth);
public:
	mat4 matrix;
	GraphicNode(mat4p matrix = mat4(1));
	~GraphicNode();
	void render();
	void attach(GraphicNode *child);
	void detachFromParent();
	template<class G> G* find(stringp name)
	{
		if (this->name == name) {
			return dynamic_cast<G*>(this);
		}
		for (auto child : children)
		{
			auto match = child->find<G>(name);
			if (match != nullptr)
			{
				return dynamic_cast<G*>(child);
			}
		}
		return nullptr;
	}
	void forEach(std::function<void(GraphicNode*, mat4p, int)> callback);
};

}
