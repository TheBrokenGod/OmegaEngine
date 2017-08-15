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

	const std::string Source::ClearBufferFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

		layout(std430, binding=0) buffer LLsHeadsBuffer
		{
			int linkedListsHeads[];
		};

		uniform int width;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		void main()
		{
			// Index based on pixel coords
			int index = getPixelBufferIndex();

			// -1 means no fragments list
			linkedListsHeads[index] = -1;

			// Don't touch backbuffer
			discard;
		}
	)";

	const std::string Source::RenderToListsVert = R"(
		#version 430 core

		layout(location = 0) in vec3 vertexPosition;
		layout(location = 1) in vec3 vertexNormal;
		layout(location = 2) in vec2 vertexTexCoord;

		uniform mat4 renderMatrix;
		uniform mat4 modelvMatrix;
		uniform mat3 normalMatrix;

		out vec3 normal;
		out vec2 texCoord;

		void main()
		{
			gl_Position = renderMatrix * vec4(vertexPosition, 1.0);
			normal = normalMatrix * vertexNormal;
			texCoord = vertexTexCoord;
		}
	)";

	const std::string Source::RenderToListsFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

		layout(binding=0) uniform atomic_uint nodeIndex;
		
		layout(std430, binding=0) buffer LLsHeadsBuffer
		{
			int listsHeads[];
		};

		struct FragmentNode {
			vec4 color;
			float depth;
			int meshId;
			int nextNode;
		};
		layout(std140, binding=1) buffer LLsNodesBuffer
		{
			FragmentNode listsNodes[];
		};

		in vec3 normal;
		in vec2 texCoord;

		uniform int width;
		uniform sampler2D sampler;
		uniform int meshId;
		uniform struct Material {
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			vec3 emissive;
			float shininess;
		} material;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		void main()
		{
			// Get unique index
			int index = int(atomicCounterIncrement(nodeIndex));

			// Write fragment data
			listsNodes[index].color = texture(sampler, texCoord);
			listsNodes[index].depth = gl_FragCoord.z;
			listsNodes[index].meshId = meshId;
			
			// Insert into pixel's linked list
			int pixelIndex = getPixelBufferIndex();			
			int nextNode = atomicExchange(listsHeads[pixelIndex], index);
			listsNodes[index].nextNode = nextNode;
		}
	)";

	const std::string Source::SortAndBlendFrag = R"(
		#version 430 core

		layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;
		
		layout(std430, binding=0) buffer LLsHeadsBuffer
		{
			int listsHeads[];
		};

		struct FragmentNode {
			vec4 color;
			float depth;
			int meshId;
			int nextNode;
		};
		layout(std140, binding=1) buffer LLsNodesBuffer
		{
			FragmentNode listsNodes[];
		};

		uniform int width;
		uniform vec3 clearColor;

		out vec3 color;

		int getPixelBufferIndex()
		{
			return int(gl_FragCoord.y * width + gl_FragCoord.x);
		}

		void main()
		{
			int listBegin = listsHeads[getPixelBufferIndex()];
			vec3 outputColor = clearColor;

			if(listBegin != -1)
			{
				// Sort linked list using bubble sort
				bool swapped;
				do
				{
					// Start at list head
					swapped = false;
					int previousNode = listBegin;
					int currentNode = listsNodes[listBegin].nextNode;

					while(currentNode != -1)
					{
						// Furthest first
						float previousDepth = listsNodes[previousNode].depth;
						float currentDepth = listsNodes[currentNode].depth;

						// TODO fix crash on nVidia
						if(previousDepth < currentDepth)
						{
							swapped = true;
							FragmentNode temp;
							temp.color = listsNodes[currentNode].color;
							temp.depth = currentDepth;
							temp.meshId = listsNodes[currentNode].meshId;
							listsNodes[currentNode].color = listsNodes[previousNode].color;
							listsNodes[currentNode].depth = previousDepth;
							listsNodes[currentNode].meshId = listsNodes[previousNode].meshId;
							listsNodes[previousNode].color = temp.color;
							listsNodes[previousNode].depth = temp.depth;
							listsNodes[previousNode].meshId = temp.meshId;
						}
						previousNode = currentNode;
						currentNode = listsNodes[currentNode].nextNode;					
					}
				}
				while(swapped);
			}

			// Blend back to front
			int currentNode = listBegin;
			while(currentNode != -1)
			{	
				vec4 fragColor = listsNodes[currentNode].color;
				float a = fragColor.a;
				outputColor = a * fragColor.rgb + (1-a) * outputColor;
				currentNode = listsNodes[currentNode].nextNode;
			}

			// Write to bb
			color = outputColor;
		}
	)";

	// Probably more than one fragment per pixel
	size_t Source::FragmentsBufferSizeFactor = 4;

	// Alignment is 4N
	struct FragmentNodeAlignedStd140 {
		vec4 color;
		float depth;
		int meshId;
		int nextNode;
		int _alignement;
	};
	size_t Source::SizeofFragmentNodeStd140 = sizeof(struct FragmentNodeAlignedStd140);

	// Alignment is N
	size_t Source::SizeofLinkedListHeadStd430 = sizeof(int);
}