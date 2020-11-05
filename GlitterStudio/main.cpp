#include <stdio.h>
#include <exception>
#include "Editor.h"

int main(int argc, char* argv[])
{
	try
	{
		Editor app;
		if (argc > 1)
		{
			for (int i = 1; i < argc; ++i)
				app.openGlitterFile(argv[i]);
		}
		app.go();
	}
	catch (std::exception e)
	{
		printf("%s", e.what());
	}
	
	return 0;
}