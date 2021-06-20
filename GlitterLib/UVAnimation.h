#pragma once
#include <string>
#include <vector>
#include "BinaryReader.h"
#include "Animation.h"
#include "UVAnimationSet.h"

namespace Glitter
{
	class UVAnimationSet;

	class UVAnimation : public Animation
	{
	private:
		std::string material;
		std::string texset;
		std::vector<UVAnimationSet> animationSets;

	public:
		UVAnimation(const std::string& filename);
		
		void readAnimations(BinaryReader &reader);
		UVAnimationSet& getAnimationSet(const std::string& name = "");

		std::string getMaterial() const;
		std::string getTexset() const;

		void read(BinaryReader& reader);
	};
}