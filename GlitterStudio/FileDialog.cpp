#include "FileDialog.h"
#include <Windows.h>
#include <shobjidl_core.h>
#include <string>

namespace Glitter
{
	namespace Editor
	{
		const char* FileDialog::getTitle(FileType type)
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

		const char* FileDialog::getFilters(FileType type)
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

		bool FileDialog::openFolderDialog(std::string& path)
		{
			HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
			if (SUCCEEDED(hr))
			{
				IFileOpenDialog* dialog = NULL;

				hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
					IID_IFileOpenDialog, reinterpret_cast<void**>(&dialog));

				if (SUCCEEDED(hr))
				{
					// Show the Open dialog box.
					dialog->SetOptions(FOS_PICKFOLDERS);
					hr = dialog->Show(NULL);

					// Get the file name from the dialog box.
					if (SUCCEEDED(hr))
					{
						IShellItem* pItem;
						hr = dialog->GetResult(&pItem);
						if (SUCCEEDED(hr))
						{
							PWSTR name;
							pItem->GetDisplayName(SIGDN_FILESYSPATH, &name);

							if (name)
							{
								std::wstring wPath = name;

								// get the size needed to convert wstring to string
								int size = WideCharToMultiByte(CP_UTF8, 0, &wPath[0], (int)wPath.size(), NULL, 0, NULL, NULL);

								// 'empty' string with required size
								path = std::string(size, 0);

								// convert wstring to string
								WideCharToMultiByte(CP_UTF8, 0, &wPath[0], (int)wPath.size(), &path[0], size, NULL, NULL);
							}

							pItem->Release();
						}
					}
					dialog->Release();
				}
				CoUninitialize();
			}

			return true;
		}

		bool FileDialog::openFileDialog(FileType type, std::string& name)
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

		bool FileDialog::saveFileDialog(FileType type, std::string& name)
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
	}
}
