#pragma once

#include <string>
#include "Engine.h"

namespace omega {

class Source
{
	friend Engine;
private:
	// Pipeline 1: opaque meshes
	static const std::string FullViewportVert;
	static const std::string ClearBuffersFrag;
	static const std::string OpaqueToGBufferVert;
	static const std::string OpaqueToGBufferFrag;
	static const std::string GBufferToColorBufferVert;
	static const std::string GBufferToColorBufferFrag;
	// Pipeline 2: transparent meshes and final blend
private:
	// SSBO base sizes
	static size_t SizeofGBufferFragment;
	static size_t LLDataBufferSizeFactor;
	static size_t SizeofLinkedListFragment;
};

}