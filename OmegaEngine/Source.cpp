#include "Source.h"

namespace omega {

	const std::string Source::FullViewportVert = R"(
		#version 430 core

		layout(location = 0) in vec2 position;

		void main()
		{
			gl_Position = vec4(position, 0.0, 1.0);
		}
	)";

	const std::string Source::ClearBuffersFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

		struct GBufferFragment {
			vec3 position;
			vec3 normal;
			vec3 texel;
			vec3 outputColor;
			uint materialId;
		};
		layout(std430, binding=0) buffer GBuffer
		{
			GBufferFragment gFragments[];
		};

		layout(std430, binding=1) buffer LLsHeadsBuffer
		{
			int linkedListsHeads[];
		};

		uniform int width;
		uniform vec3 clearColor;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		out vec3 color;

		void main()
		{
			int index = getPixelBufferIndex();

			// Null normal means no fragment
			gFragments[index].normal = vec3(0, 0, 0);

			// Output color is used for final blending
			gFragments[index].outputColor = clearColor;

			// -1 means no linked list
			linkedListsHeads[index] = -1;

			// Don't touch backbuffer
			discard;
		}
	)";

	const std::string Source::OpaqueToGBufferVert = R"(
		#version 430 core

		layout(location = 0) in vec3 vertexPosition;
		layout(location = 1) in vec3 vertexNormal;
		layout(location = 2) in vec2 vertexTexCoord;

		uniform mat4 renderMatrix;
		uniform mat4 modelvMatrix;
		uniform mat3 normalMatrix;

		out vec3 position;
		out vec3 normal;
		out vec2 texCoord;

		void main()
		{
			gl_Position = renderMatrix * vec4(vertexPosition, 1.0);
			position = (modelvMatrix * vec4(vertexPosition, 1.0)).xyz;
			normal = normalMatrix * vertexNormal;
			texCoord = vertexTexCoord;
		}
	)";

	const std::string Source::OpaqueToGBufferFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

		in vec3 position;
		in vec3 normal;
		in vec2 texCoord;


		struct GBufferFragment {
			vec3 position;
			vec3 normal;
			vec3 texel;
			vec3 outputColor;
			uint materialId;
		};
		layout(std430, binding=0) buffer GBuffer
		{
			GBufferFragment gFragments[];
		};

		uniform sampler2D sampler;
		uniform int width;
		uniform int materialId;

		// TODO remove
		out vec3 color;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		void main()
		{
			GBufferFragment fragment;
			fragment.position = position;
			fragment.normal = normalize(normal);
			fragment.materialId = materialId;
			fragment.texel = texture(sampler, texCoord).rgb;
			fragment.outputColor = vec3(0, 0, 0);
			gFragments[getPixelBufferIndex()] = fragment;

			// TODO remove
			//color = texture(sampler, texCoord).rgb;
			discard;
		}
	)";

	const std::string Source::ApplyLightingFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

		struct GBufferFragment {
			vec3 position;
			vec3 normal;
			vec3 texel;
			vec3 outputColor;
			uint materialId;
		};
		layout(std430, binding=0) buffer GBuffer
		{
			GBufferFragment gFragments[];
		};

		uniform int width;

		out vec3 color;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		void main()
		{
			int index = getPixelBufferIndex();

			if(length(gFragments[index].normal) > 0)
			{
				gFragments[index].outputColor = gFragments[index].texel;
			}
			
			//discard;
			color = gFragments[index].outputColor;
		}
	)";

	struct AlignedGBufferFragment {
		vec4 position;
		vec4 normal;
		vec4 texel;
		vec4 outputColor;
		vec4 others;
	};

	size_t Source::SizeofAlignedGBufferFragment = sizeof(struct AlignedGBufferFragment);

	size_t Source::LLDataBufferSizeFactor = 4;

	// TODO alignment
	struct LinkedListFragment {
		float color[4];
		float depth;
		int previousNode;
	};

	size_t Source::SizeofLinkedListFragment = sizeof(struct LinkedListFragment);
}