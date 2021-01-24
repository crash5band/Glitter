#include <stdio.h>
#include <exception>
#include <filesystem>
#include "Editor.h"

int main(int argc, char* argv[])
{
	try
	{
		Editor app;
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
		printf("%s", e.what());
	}
	
	return 0;
}
