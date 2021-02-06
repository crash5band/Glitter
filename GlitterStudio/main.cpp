#include <stdio.h>
#include <exception>
#include <filesystem>
#include "Editor.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
	try
	{
		Editor app(argv[0]);

		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
			{
				if (std::filesystem::is_directory(argv[i]))
					app.openFolder(argv[i]);
				else
					app.openGlitterFile(argv[i]);
			}
		}
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
