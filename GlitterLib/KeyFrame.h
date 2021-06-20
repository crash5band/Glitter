#pragma once
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	struct Keyframe
	{
		float frame;
		float value;

		Keyframe() : frame{ 0 }, value{ 0 }
		{

		}

		void read(BinaryReader &reader)
		{
			frame = reader.readSingle();
			value = reader.readSingle();
		}

		void write(BinaryWriter &writer)
		{
			writer.writeSingle(frame);
			writer.writeSingle(value);
		}
	};
}