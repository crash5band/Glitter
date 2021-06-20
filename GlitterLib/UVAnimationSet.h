#pragma once
#include "BinaryReader.h"
#include "KeyFrameSet.h"
#include "AnimationSet.h"
#include <vector>
namespace Glitter
{
	constexpr int uAnim = 0x00010000;
	constexpr int vAnim = 0x01010000;
	constexpr int uAnim2 = 0x00000000;
	constexpr int vAnim2 = 0x01000000;

	class UVAnimationSet : public AnimationSet
	{
	public:
		void read(BinaryReader& reader, char* names, std::vector<Keyframe> keyframes);
	};
}