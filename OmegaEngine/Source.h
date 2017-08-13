#pragma once

#include <string>
#include "Engine.h"

namespace omega {

class Source
{
	friend Engine;
private:
	static const std::string FullViewportVert;
	// Pipeline 1: opaque meshes
	static const std::string ClearBuffersFrag;
	static const std::string OpaqueToGBufferVert;
	static const std::string OpaqueToGBufferFrag;
	static const std::string ApplyLightingFrag;
	// Pipeline 2: transparent meshes and blend
	// TODO
private:
	// SSBO base sizes
	static size_t SizeofAlignedGBufferFragment;
	static size_t LLDataBufferSizeFactor;
	static size_t SizeofLinkedListFragment;
};

}