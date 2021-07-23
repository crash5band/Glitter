#pragma once
#include "GlitterPlayer.h"
#include "Inspector.h"

namespace Glitter
{
	namespace Editor
	{
		enum class SelectionMode
		{
			GTE,
			GTM
		};

		class ParticleEditor
		{
		private:
			std::vector<std::shared_ptr<EffectNode>> effects;
			int selectedEffect;
			int selectedChild;
			int selectedMaterial;
			SelectionMode selectionMode;
			bool previewMaterials;

			Inspector inspector;
			AnimationTimeline animationTimeline;
			GlitterPlayer player;
			std::shared_ptr<INode> nodeCopy;

			void updateTreeview();
			bool effectMenu(int index);
			bool emitterMenu(int parent, int index);
			bool particleMenu(int parent, int index);
			bool instanceMenu(int effect, int emitter, int index);
			bool materialMenu(int material);

		public:
			ParticleEditor();

			void open(const std::string& filename);
			void openFolder(const std::string& directory);
			void closeEffect(int index);
			void closeAllEffects();
			void save(int index, bool saveAs);
			void saveMaterial(int index, bool saveAs);
			void saveAll();
			void copy();
			void paste();
			void duplicateEffect(int index);
			void updateMenubar();
			void update(Renderer* renderer, float deltaT);
			void setSelectedNode();
			void cleanUp();

			void createEffect(const std::string& name);
			void createEmitter(std::shared_ptr<EffectNode> effect, const std::string& name);
			void createParticle(std::shared_ptr<EffectNode> effect, const std::string& name);
			void removeEmitter(std::shared_ptr<EffectNode> parent, int index);
			void removeParticle(std::shared_ptr<EffectNode> parent, int index);
			void addParticleInstance(std::shared_ptr<EmitterNode> parent, std::shared_ptr<ParticleNode> particle, int pos);
			void removeParticleInstance(std::shared_ptr<EmitterNode> parent, std::shared_ptr<ParticleNode> particle, int pos);
			void createMaterial(const std::string& name);

			int getSelectedEffect() const;
			int getSelectedMaterial() const;
			std::vector<std::weak_ptr<ParticleNode>> getAvailableParticles(std::shared_ptr<EffectNode> effect, std::shared_ptr<EmitterNode> emitter);
		};
	}
}
