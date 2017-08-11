#pragma once

#include "Types.hpp"

namespace omega {

class GraphicObject
{
	GraphicObject(const GraphicObject&) = delete;
	GraphicObject& operator =(const GraphicObject&) = delete;
protected:
	GraphicObject();
public:
	string name;
	virtual ~GraphicObject();
	virtual void render() = 0;
};

}
