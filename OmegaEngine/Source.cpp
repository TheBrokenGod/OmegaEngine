#include "Source.h"

namespace omega {

	const std::string Source::cleanGBufferVert = R"(
	)";

	const std::string Source::cleanGBufferFrag = R"(
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
		float workingColor[3];
	};

	size_t Source::SizeofGBufferFragment = sizeof(struct GBufferFragment);

	size_t Source::LLBufferSizeFactor = 4;

	struct LinkedListFragment {
		float color[4];
		float depth;
		int previousNode;
	};

	size_t Source::SizeofLinkedListFragment = sizeof(struct LinkedListFragment);
}