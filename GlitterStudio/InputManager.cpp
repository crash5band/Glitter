#include "InputManager.h"

void InputManager::update(GLFWwindow* window)
{
	prevState = state;
	for (int i = 0; i < GLFW_KEY_LAST; ++i)
		state[i] = glfwGetKey(window, i);
}

bool InputManager::isTapped(int key)
{
	return !prevState[key] && state[key];
}

bool InputManager::isDown(int key)
{
	return state[key];
}

bool InputManager::isUp(int key)
{
	return !state[key];
}
