#pragma once
#include "ICommand.h"
#include "EditorAnimationSet.h"
#include <memory>

namespace Glitter
{
	namespace Editor
	{
		
		class AddAnimationCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			EditorAnimation animation;
			size_t position;

		public:
			AddAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, EditorAnimation a, size_t pos);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Add Animation"; }
			bool isValid() override { return !set.expired(); }
		};

		class RemoveAnimationCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			EditorAnimation animation;
			size_t position;

		public:
			RemoveAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t pos);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Remove Animation"; }
			bool isValid() override { return !set.expired(); }
		};

		class ChangeAnimationCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			EditorAnimation newValue;
			EditorAnimation oldValue;
			size_t position;

		public:
			ChangeAnimationCommand(std::shared_ptr<EditorAnimationSet>& aSet, EditorAnimation& newValue, size_t pos);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Change Animation"; }
			bool isValid() override { return !set.expired(); }
		};

		class AddKeyCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			size_t animIndex;
			size_t keyPos;
			EditorKeyframe key;

		public:
			AddKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, int frame);
			AddKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, EditorKeyframe k);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Add Key"; }
			bool isValid() override { return !set.expired(); }
		};

		class RemoveKeyCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			size_t animIndex;
			size_t keyPos;
			EditorKeyframe key;

		public:
			RemoveKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, size_t pos);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Remove Key"; }
			bool isValid() override { return !set.expired(); }
		};

		class ChangeKeyCommand : public ICommand
		{
		private:
			std::weak_ptr<EditorAnimationSet> set;
			size_t animIndex;
			size_t keyPos;
			EditorKeyframe newValue;
			EditorKeyframe oldValue;

		public:
			ChangeKeyCommand(std::shared_ptr<EditorAnimationSet>& aSet, size_t aIndex, EditorKeyframe& newValue, size_t pos);

			void execute() override;
			void undo() override;
			const char* getDescription() override { return "Change Key Frame"; }
			bool isValid() override { return !set.expired(); }
		};
	}
}
