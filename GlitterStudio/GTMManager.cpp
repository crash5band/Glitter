#include "GTMManager.h"
#include "IconsFontAwesome5.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "UI.h"
#include "Logger.h"
#include "File.h"
#include "ResourceManager.h"

namespace Glitter
{
	namespace Editor
	{
		std::vector<std::shared_ptr<MaterialNode>> GTMManager::materialNodes;

		void GTMManager::add(std::shared_ptr<MaterialNode>& node)
		{
			materialNodes.emplace_back(node);
		}

		void GTMManager::remove(size_t pos)
		{
			std::string matName = materialNodes.at(pos)->getMaterial()->getName();
			if (materialNodes.at(pos).use_count() < 2)
			{
				materialNodes.erase(materialNodes.begin() + pos);
				ResourceManager::cleanTextures();
				Logger::log(Message(MessageType::Normal, "Closed material " + matName + "."));
			}
			else
			{
				std::string warning = "Cannot close " + matName + ". The material is used by on open effect.";
				Logger::log(Message(MessageType::Warning, warning));
			}
		}

		void GTMManager::clean()
		{
			// Dispose of unused materials
			for (std::vector<std::shared_ptr<MaterialNode>>::iterator it = materialNodes.begin(); it != materialNodes.end();)
			{
				if ((*it).use_count() < 2)
				{
					std::string matName = (*it)->getMaterial()->getName();
					it = materialNodes.erase(it);
					Logger::log(Message(MessageType::Normal, "Closed material " + matName + "."));
				}
				else
					++it;
			}
		}

		void GTMManager::clear()
		{
			materialNodes.clear();
			ResourceManager::cleanTextures();
		}

		void GTMManager::save(int index, bool saveAs)
		{
			if (index < 0 || index >= count())
				return;

			std::string name = materialNodes[index]->getMaterial()->getFilename();
			if (name.size() && !saveAs)
			{
				materialNodes[index]->getMaterial()->write(name);
			}
			else
			{
				name = materialNodes[index]->getMaterial()->getName();
				if (FileGUI::saveFileGUI(FileType::Material, name))
				{
					if (Glitter::File::getFileExtension(name) != "gtm")
						name += ".gtm";

					materialNodes[index]->getMaterial()->write(name);
				}
			}

			Logger::log(Message(MessageType::Normal, "Saved material " + name));
		}

		void GTMManager::saveAll()
		{

		}

		void GTMManager::duplicate(int index)
		{
			std::shared_ptr<MaterialNode> newNode = std::make_shared<MaterialNode>(materialNodes[index]);
			add(newNode);
		}

		std::vector<std::shared_ptr<MaterialNode>> GTMManager::getNodes()
		{
			return materialNodes;
		}

		int GTMManager::count()
		{
			return materialNodes.size();
		}
	}
}
