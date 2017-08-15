#pragma once

#include <string>
#include "Engine.h"

namespace omega {

class Source
{
	friend Engine;
private:
	static const std::string FullViewportVert;
	static const std::string ClearBufferFrag;
	static const std::string RenderToListsVert;
	static const std::string RenderToListsFrag;
	static const std::string SortAndBlendFrag;
private:
	static size_t SizeofFragmentNodeStd140;
	static size_t FragmentsBufferSizeFactor;
	static size_t SizeofLinkedListHeadStd430;
};

}