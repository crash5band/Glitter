#include "ParticleEditor.h"
#include "GTMManager.h"
#include "CommandManager.h"
#include "ResourceManager.h"
#include "File.h"
#include "FileGUI.h"
#include "IconsFontAwesome5.h"
#include "UI.h"
#include <filesystem>

namespace Glitter
{
	namespace Editor
	{
		ParticleEditor::ParticleEditor() :
			selectedEffect{ -1 }, selectedChild{ -1 }, selectedMaterial{ -1 }, selectionMode{ SelectionMode::GTE },
			animationTimeline{ AnimationTimeline(UI::pTimelineWindow) }, previewMaterials{ true }
		{

		}

		int ParticleEditor::getSelectedEffect() const
		{
			return selectedEffect;
		}

		int ParticleEditor::getSelectedMaterial() const
		{
			return selectedMaterial;
		}
		
		void ParticleEditor::open(const std::string& filename)
		{
			if (!Glitter::File::exists(filename))
			{
				return;
			}

			std::string extension = Glitter::File::getFileExtension(filename);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
			if (extension == "gte")
			{
				auto effect = std::make_shared<Glitter::GlitterEffect>(filename);
				auto effectNode = std::make_shared<EffectNode>(effect);
				effects.emplace_back(effectNode);

				std::string filepath = Glitter::File::getFilePath(filename);

				for (auto& particle : effect->getParticles())
					open(filepath + particle->getMaterial() + ".gtm");

				std::vector<std::shared_ptr<MaterialNode>> materials = GTMManager::getNodes();
				for (auto& p : effectNode->getParticleNodes())
				{
					for (auto& m : materials)
					{
						if (filepath == Glitter::File::getFilePath(m->getMaterial()->getFilename())
							&& File::getFileNameWithoutExtension(m->getMaterial()->getFilename()) == p->getParticle()->getMaterial())
							p->setMaterial(m);
					}
				}
			}
			else if (extension == "gtm")
			{
				std::vector<std::shared_ptr<MaterialNode>> materials = GTMManager::getNodes();
				for (const auto& m : materials)
				{
					if (m->getMaterial()->getFilename() == filename)
						return;
				}

				auto material = std::make_shared<Glitter::GlitterMaterial>(filename);
				auto materialNode = std::make_shared<MaterialNode>(material);
				GTMManager::add(materialNode);
			}
		}

		void ParticleEditor::openFolder(const std::string& directory)
		{
			if (!std::filesystem::exists(directory))
				return;

			std::vector<std::string> files;
			for (const auto& file : std::filesystem::directory_iterator(directory))
			{
				std::string extension = file.path().extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				if (extension == ".gte")
					files.emplace_back(file.path().string());
			}

			effects.reserve(effects.size() + files.size());
			for (const std::string& file : files)
				open(file);
		}

		void ParticleEditor::closeEffect(int index)
		{
			if (index < 0 || index >= effects.size())
				return;

			effects.erase(effects.begin() + index);
			selectedEffect = selectedChild = -1;

			inspector.setNode(nullptr);
			animationTimeline.setAnimations(nullptr);
			player.setEffect(nullptr);
			cleanUp();
		}

		void ParticleEditor::closeAllEffects()
		{
			effects.clear();
			selectedEffect = selectedChild = -1;

			inspector.setNode(nullptr);
			animationTimeline.setAnimations(nullptr);
			player.setEffect(nullptr);
			cleanUp();
		}

		// TODO: optimize this
		void ParticleEditor::save(int index, bool saveAs)
		{
			if (index > -1 && index < effects.size())
			{
				std::string name = effects[index]->getEffect()->getFilename();
				bool ok = false;
				if (name.size() && !saveAs)
				{
					ok = true;
				}
				else
				{
					name = effects[index]->getEffect()->getName();
					if (FileGUI::saveFileGUI(FileType::Effect, name))
					{
						if (Glitter::File::getFileExtension(name) != "gte")
							name += ".gte";

						ok = true;
					}
				}

				if (ok)
				{
					effects[index]->save(name);

					// save materials used by effect's particles
					auto& particles = effects[index]->getParticleNodes();
					auto materials = GTMManager::getNodes();
					for (auto& particle : particles)
					{
						for (int m = 0; m < materials.size(); ++m)
						{
							if (particle->getMaterialNode())
							{
								if (particle->getMaterialNode()->getMaterial()->getName() == materials[m]->getMaterial()->getName())
									GTMManager::save(m, false);
							}
						}
					}
				}
			}
		}

		void ParticleEditor::saveAll()
		{
			for (int e = 0; e < effects.size(); ++e)
				save(e, false);
		}

		void ParticleEditor::saveMaterial(int index, bool saveAs)
		{
			GTMManager::save(index, saveAs);
		}

		void ParticleEditor::copy()
		{
			if (selectedEffect >= 0 && selectedEffect < effects.size())
			{
				int emitterCount = effects[selectedEffect]->getEmitterNodes().size();
				int particleCount = effects[selectedEffect]->getParticleNodes().size();

				if (selectedChild == -1)
					nodeCopy = effects[selectedEffect];
				else if (selectedChild >= 0 && selectedChild < emitterCount)
					nodeCopy = effects[selectedEffect]->getEmitterNodes().at(selectedChild);
				else if (selectedChild >= emitterCount && selectedChild < emitterCount + particleCount)
					nodeCopy = effects[selectedEffect]->getParticleNodes().at(selectedChild - emitterCount);
			}
		}

		void ParticleEditor::paste()
		{
			std::shared_ptr<INode> n = nodeCopy;
			if (!n)
				return;

			if (selectedEffect < 0 || selectedEffect >= effects.size())
				return;

			if (n->getNodeType() == NodeType::Emitter)
			{
				std::shared_ptr<EmitterNode> curNode = std::static_pointer_cast<EmitterNode>(n);
				std::shared_ptr<EmitterNode> newNode = std::make_shared<EmitterNode>(curNode);
				std::vector<std::shared_ptr<ParticleNode>> copyParticles;

				// remove current emitter particle references in case we copy to a different effect
				int count = curNode->getParticles().size();
				for (int i = 0; i < count; ++i)
				{
					copyParticles.push_back(curNode->getParticles().at(i).getReference());
					newNode->getParticles().erase(newNode->getParticles().begin());
					newNode->getEmitter()->removeParticle(0);
				}

				// look for missing particles in target effect
				auto efParticles = effects[selectedEffect]->getEffect()->getParticles();
				for (int i = 0; i < copyParticles.size(); ++i)
				{
					bool found = false;
					for (int j = 0; j < efParticles.size(); ++j)
					{
						if (copyParticles[i]->getParticle() == efParticles[j])
						{
							found = true;
							break;
						}
					}

					// particle does not exist in selected effect
					if (!found)
					{
						// add missing particle to target effect and new emitter
						std::shared_ptr<ParticleNode> pNode = std::make_shared<ParticleNode>(copyParticles[i]);
						effects[selectedEffect]->getParticleNodes().push_back(pNode);
						effects[selectedEffect]->getEffect()->addParticle(pNode->getParticle());
						newNode->getParticles().push_back(ParticleInstance(pNode));
						newNode->getEmitter()->addParticle(pNode->getParticle());
					}
					else
					{
						newNode->getParticles().push_back(ParticleInstance(copyParticles[i]));
						newNode->getEmitter()->addParticle(copyParticles[i]->getParticle());
					}
				}

				effects[selectedEffect]->getEffect()->addEmitter(newNode->getEmitter());
				effects[selectedEffect]->getEmitterNodes().push_back(newNode);
			}
			else if (n->getNodeType() == NodeType::Particle)
			{
				std::shared_ptr<ParticleNode> curNode = std::static_pointer_cast<ParticleNode>(n);
				std::shared_ptr<ParticleNode> newNode = std::make_shared<ParticleNode>(curNode);

				effects[selectedEffect]->getParticleNodes().push_back(newNode);
				effects[selectedEffect]->getEffect()->addParticle(newNode->getParticle());
			}
		}

		void ParticleEditor::duplicateEffect(int index)
		{
			effects.push_back(std::make_shared<EffectNode>(effects[index]));
		}

		void ParticleEditor::updateMenubar()
		{
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl + O"))
				{
					std::string name;
					if (FileGUI::openFileGUI(FileType::Effect, name))
						open(name);
				}

				if (ImGui::MenuItem("Open Folder"))
				{
					std::string path;
					if (FileGUI::openFolderGUI(path))
						openFolder(path);
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Save", "Ctrl + S"))
					save(selectedEffect, false);

				if (ImGui::MenuItem("Save As...", "Ctrl + Shift + S"))
					save(selectedEffect, true);

				if (ImGui::MenuItem("Save All"))
					saveAll();

				if (ImGui::MenuItem("Close All"))
					closeAllEffects();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("New Effect"))
					createEffect("effect");

				if (ImGui::MenuItem("New Emitter"))
				{
					if (selectedEffect > - 1 && selectedEffect < effects.size())
						createEmitter(effects[selectedEffect], "emitter");
				}

				if (ImGui::MenuItem("New Particle"))
				{
					if (selectedEffect > -1 && selectedEffect < effects.size())
						createParticle(effects[selectedEffect], "particle");
				}

				if (ImGui::MenuItem("New Material"))
					createMaterial("material");

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_COPY " Copy", "Ctrl + C"))
					copy();

				if (ImGui::MenuItem(ICON_FA_PASTE " Paste", "Ctrl + V"))
					paste();

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Preview Materials", NULL, &previewMaterials);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Materials"))
			{

				if (ImGui::MenuItem("Open", "Alt + O"))
				{
					std::string name;
					if (FileGUI::openFileGUI(FileType::Material, name))
						open(name);
				}

				if (ImGui::MenuItem("Save", "Alt + S"))
					GTMManager::save(selectedMaterial, false);

				if (ImGui::MenuItem("Save As...", "Shift + Alt + S"))
					GTMManager::save(selectedMaterial, true);

				ImGui::EndMenu();
			}

#ifdef _DEBUG
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Dump Effect XML"))
				{
					if (selectedEffect >= 0 && selectedEffect < effects.size())
					{
						std::string filename = effects[selectedEffect]->getEffect()->getFilename();
						if (filename.size())
							filename = File::getFileNameWithoutExtension(filename).append(".xml");
						else
							filename = effects[selectedEffect]->getEffect()->getName().append(".xml");

						if (filename.size())
							effects[selectedEffect]->getEffect()->writeXML(filename);
					}
				}

				if (ImGui::MenuItem("Dump Material XML"))
				{
					if (selectedMaterial >= 0 && selectedMaterial < GTMManager::count())
					{
						std::vector<std::shared_ptr<MaterialNode>> materialNodes = GTMManager::getNodes();
						std::string filename = materialNodes[selectedMaterial]->getMaterial()->getFilename();
						if (filename.size())
							filename = File::getFileNameWithoutExtension(filename).append(".xml");
						else
							filename = materialNodes[selectedMaterial]->getMaterial()->getName().append(".xml");

						if (filename.size())
							materialNodes[selectedMaterial]->getMaterial()->writeXML(filename);
					}
				}

				ImGui::EndMenu();
			}
#endif // _DEBUG

			ImGui::EndMainMenuBar();
		}

		void ParticleEditor::update(Renderer* renderer, float deltaT)
		{
			updateMenubar();
			updateTreeview();
			inspector.update();
			animationTimeline.update();
			player.update(renderer, deltaT);
		}

		void ParticleEditor::cleanUp()
		{
			GTMManager::clean();
			CommandManager::clean();
			ResourceManager::cleanModels();
			ResourceManager::cleanTextures();
		}
	}
}