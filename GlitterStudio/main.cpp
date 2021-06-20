#include <stdio.h>
#include <exception>
#include <filesystem>
#include "Application.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
	try
	{
		Glitter::Editor::Application app(argv[0]);
		app.go();
	}
	catch (std::exception e)
	{
		std::string msg("An unhandled exception has occured and the application will now close.\nError. ");
		msg.append(e.what());
		MessageBox(NULL, msg.c_str(), "Glitter Studio", MB_OK | MB_ICONERROR);
	}
	
	return 0;
}
