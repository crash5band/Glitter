#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <array>

namespace Glitter
{
	namespace Editor
	{
		class InputListener
		{
		private:
			std::array<bool, GLFW_KEY_LAST> prevState;
			std::array<bool, GLFW_KEY_LAST> state;

		public:
			void update(GLFWwindow* window);
			bool isTapped(int key);
			bool isDown(int key);
			bool isUp(int key);
		};
	}
}
