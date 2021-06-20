#include "InputListener.h"

void InputListener::update(GLFWwindow* window)
{
	prevState = state;
	for (int i = 0; i < GLFW_KEY_LAST; ++i)
		state[i] = glfwGetKey(window, i);
}

bool InputListener::isTapped(int key)
{
	return !prevState[key] && state[key];
}

bool InputListener::isDown(int key)
{
	return state[key];
}

bool InputListener::isUp(int key)
{
	return !state[key];
}
