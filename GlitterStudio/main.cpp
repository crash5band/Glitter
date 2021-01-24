#include <stdio.h>
#include <exception>
#include <filesystem>
#include "Editor.h"
#include <Windows.h>

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
		MessageBox(NULL, e.what(), "Glitter Studio", MB_OK);
	}
	
	return 0;
}
