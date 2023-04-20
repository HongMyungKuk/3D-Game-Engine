#pragma once

#include <vector>
#include <glm/glm.hpp>


namespace hmk
{
	class Rasterization
	{
	public:
		Rasterization(int width, int height);
		~Rasterization();

		void Render(const std::vector<glm::vec4>& pixels);
	};
}

