#pragma once
#include <string>
#include <array>

namespace Glitter
{
	enum class InterpolationType
	{
		Linear,
		Hermite,
		Constant
	};

	enum class AnimationType
	{
		Tx,
		Ty,
		Tz,
		Rx,
		Ry,
		Rz,
		Sx,
		Sy,
		Sz,
		SAll,
		ColorR,
		ColorG,
		ColorB,
		ColorA,
		UScroll,
		VScroll,
		UScrollAlpha,
		VScrollAlpha,
		SecondaryUScroll,
		SecondaryVScroll,
		SecondaryUScrollAlpha,
		SecondaryVScrollAlpha,
		EmissionInterval,
		ParticlePerEmission
	};

	enum class RepeatType
	{
		Constant,
		Repeat
	};

	enum class EmitterType
	{
		Box,
		Cylinder,
		Sphere,
		Mesh,
		Polygon,
	};

	enum class EmitCondition
	{
		Time,
		MovingDistance
	};

	enum class EmissionDirectionType
	{
		Inward,
		Outward,
		ParticleVelocity
	};

	enum class ParticleType
	{
		Quad,
		Mesh,
		Locus,
		Line
	};

	enum class PivotPosition
	{
		TopLeft,
		TopCenter,
		TopRight,
		MiddleLeft,
		MiddleCenter,
		MiddleRight,
		BottomLeft,
		BottomCenter,
		BottomRight,
	};

	enum class EmitterDirectionType
	{
		ParentAxis,
		Billboard,
		XAxis,
		YAxis,
		ZAxis,
		YRotationOnly,
	};

	enum class ParticleDirectionType
	{
		Billboard,
		DirectionalAngle,
		DirectionalAngleBillboard,
		EmitterDirection,
		EmitterAxis,
		EmittedEmitterAxis, // child emitter?
		XAxis,
		YAxis,
		ZAxis,
		YRotationOnly,
	};

	enum class UVIndexType
	{
		Fixed,
		InitialRandom,
		InitialRandomReverseOrder,
		InitialRandomSequentialOrder,
		RandomOrder,
		ReverseOrder,
		SequentialOrder,
		User
	};

	enum class BlendMode
	{
		// Particles use all. Materials use all but UseMaterial.

		Zero,
		Typical,
		Add,
		Subtract,
		Multiply,
		PunchThrough,
		Opaque,
	};

	enum class AddressMode
	{
		Clamp,
		Wrap,
		UseMaterial
	};

	const size_t interpolationTypeTableSize = 3;
	const std::string interpolationTypeTable[] =
	{
		"Linear",
		"Hermite",
		"Constant"
	};

	const size_t animationTypeTableSize = 24;
	const std::string animationTypeTable[] =
	{
		"Tx",
		"Ty",
		"Tz",
		"Rx",
		"Ry",
		"Rz",
		"Sx",
		"Sy",
		"Sz",
		"SAll",
		"ColorR",
		"ColorG",
		"ColorB",
		"ColorA",
		"UScroll",
		"VScroll",
		"UScrollAlpha",
		"VScrollAlpha",
		"SecondaryUScroll",
		"SecondaryVScroll",
		"SecondaryUScrollAlpha",
		"SecondaryVScrollAlpha",
		"EmissionInterval",
		"ParticlePerEmission"
	};

	const size_t repeatTypeTableSize = 2;
	const std::string repeatTypeTable[] =
	{
		"Constant",
		"Repeat"
	};

	const size_t emitterTypeTableSize = 5;
	const std::string emitterTypeTable[] =
	{
		"Box",
		"Cylinder",
		"Sphere",
		"Mesh",
		"Polygon"
	};

	const size_t emitConditionTableSize = 2;
	const std::string emitConditionTable[] =
	{
		"Time",
		"MovingDistance"
	};

	const size_t emissionDirectionTypeTableSize = 3;
	const std::string emissionDirectionTypeTable[] =
	{
		"Inward",
		"Outward",
		"ParticleVelocity"
	};

	const size_t particleTypeTableSize = 4;
	const std::string particleTypeTable[] =
	{
		"Quad",
		"Mesh",
		"Locus",
		"Line"
	};

	const size_t pivotPositionTableSize = 9;
	const std::string pivotPositionTable[] =
	{
		"TopLeft",
		"TopCenter",
		"TopRight",
		"MiddleLeft",
		"MiddleCenter",
		"MiddleRight",
		"BottomLeft",
		"BottomCenter",
		"BottomRight"
	};

	const size_t PdirectionTypeTableSize = 10;
	const std::string PdirectionTypeTable[] =
	{
		"Billboard",
		"DirectionalAngle",
		"DirectionalAngleBillboard",
		"EmitterDirection",
		"EmitterAxis",
		"EmittedEmitterAxis",
		"XAxis",
		"YAxis",
		"ZAxis",
		"YRotationOnly"
	};

	const size_t EdirectionTypeTableSize = 6;
	const std::string EdirectionTypeTable[] =
	{
		"ParentAxis",
		"Billboard",
		"XAxis",
		"YAxis",
		"ZAxis",
		"YRotationOnly"
	};

	const size_t uvIndexTypeTableSize = 8;
	const std::string uvIndexTypeTable[] =
	{
		"Fixed",
		"InitialRandom",
		"InitialRandomReverseOrder",
		"InitialRandomSequentialOrder",
		"RandomOrder",
		"ReverseOrder",
		"SequentialOrder",
		"User"
	};

	const size_t blendModeTableSize = 7;
	const std::string blendModeTable[] =
	{
		"Zero",
		"Typical",
		"Add",
		"Subtract",
		"Multiply",
		"PunchThrough",
		"Opaque"
	};

	const size_t addressModeTableSize = 3;
	const std::string addressModeTable[] =
	{
		"Clamp",
		"Wrap",
		"UseMaterial"
	};

	static std::string glitterEnumToString(const std::string* arr, size_t size, size_t index)
	{
		return index < size ? arr[index] : "UnknownParameter";
	}

	static size_t glitterStringToEnum(const std::string* arr, size_t size, const std::string& str)
	{
		for (size_t index = 0; index < size; ++index)
		{
			if (arr[index] == str)
				return index;
		}

		return 0;
	}
}