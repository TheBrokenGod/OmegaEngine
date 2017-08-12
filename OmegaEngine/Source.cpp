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
			float position[3];
			float normal[3];
			float texel[3];
			float ambient[3];
			float diffuse[3];
			float specular[3];
			float emissive[3];
			float shininess;
			float outputColor[3];
		};
		layout(std430, binding=0) buffer GBuffer
		{
			GBufferFragment gFragments[];
		};

		layout(std430, binding=1) buffer LLHeadBuffer
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
			gFragments[index].normal[0] = 0;
			gFragments[index].normal[1] = 0;
			gFragments[index].normal[2] = 0;

			// Output color is used for final blending
			gFragments[index].outputColor[0] = clearColor.r;
			gFragments[index].outputColor[1] = clearColor.b;
			gFragments[index].outputColor[2] = clearColor.g;

			// -1 means no linked list
			linkedListsHeads[index] = -1;

			// Don't touch backbuffer
			discard;
		}
	)";

	const std::string Source::OpaqueToGBufferVert = R"(
		#version 430 core

		layout(location = 0) in vec3 position;
		layout(location = 1) in vec3 normal;
		layout(location = 2) in vec2 vertexTexCoord;

		uniform mat4 renderMatrix;
		uniform mat3 normalMatrix;

		//out vec4 eyePosition;
		//out vec3 eyeNormal;
		out vec2 texCoord;

		void main()
		{
			gl_Position = renderMatrix * vec4(position, 1.0);
			texCoord = vertexTexCoord;
		}
	)";

	const std::string Source::OpaqueToGBufferFrag = R"(
		#version 430 core

		uniform sampler2D sampler;

		in vec2 texCoord;

		out vec3 color;

		void main()
		{
			color = texture(sampler, texCoord).rgb;
		}
	)";

	const std::string Source::GBufferToColorBufferVert = R"(
	)";

	const std::string Source::GBufferToColorBufferFrag = R"(
	)";

	struct GBufferFragment {
		float position[3];
		float normal[3];
		float texel[3];
		float ambient[3];
		float diffuse[3];
		float specular[3];
		float emissive[3];
		float shininess;
		float outputColor[3];
	};

	size_t Source::SizeofGBufferFragment = sizeof(struct GBufferFragment);

	size_t Source::LLDataBufferSizeFactor = 4;

	struct LinkedListFragment {
		float color[4];
		float depth;
		int previousNode;
	};

	size_t Source::SizeofLinkedListFragment = sizeof(struct LinkedListFragment);
}