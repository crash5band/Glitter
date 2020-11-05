#include "FileGUI.h"
#include <Windows.h>

const char* FileGUI::getTitle(FileType type)
{
	switch (type)
	{
	case FileType::Effect:
		return "Glitter Effect";

	case FileType::Material:
		return "Glitter Material";

	case FileType::Model:
		return "Model";

	case FileType::Texture:
		return "Texture";

	default:
		return "File";
	}
}

const char* FileGUI::getFilters(FileType type)
{
	switch (type)
	{
	case FileType::Effect:
		return "Glitter Effect(.gte)\0*.gte\0";

	case FileType::Material:
		return "Glitter Material(.gtm)\0*.gtm\0";

	case FileType::Model:
		return "Model(.model)\0*.model\0";

	case FileType::Texture:
		return "Texture(.dds)\0*.dds\0";

	default:
		return "All files\0*.*\0";
	}
}

bool FileGUI::openFileGUI(FileType type, std::string& name)
{
	char* filename = (char*)malloc(1024);
	strcpy_s(filename, 1024, "");

	char title[128];
	strcpy_s(title, "Open ");
	strcat_s(title, getTitle(type));

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = getFilters(type);
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = 1024;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER
		| OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn))
	{
		name = filename;
		return true;
	}

	return false;
}

bool FileGUI::saveFileGUI(FileType type, std::string& name)
{
	char* filename = (char*)malloc(1024);
	if (name.size())
	{
		strcpy_s(filename, 1024, name.c_str());
	}
	else
	{
		strcpy_s(filename, 1024, "");
	}

	char title[128];
	strcpy_s(title, "Save ");
	strcat_s(title, getTitle(type));

	OPENFILENAME ofn;
	memset(&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFilter = getFilters(type);
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = filename;
	ofn.nMaxFile = 1024;
	ofn.lpstrTitle = title;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER
		| OFN_HIDEREADONLY | OFN_ENABLESIZING;

	if (GetSaveFileName(&ofn))
	{
		name = filename;
		return true;
	}

	return false;
}