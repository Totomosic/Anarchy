#pragma once
#include "Utils.h"

namespace Anarchy
{

#define ANCH_DEFINE_DEFAULT_DESERIALIZE(type)	\
	inline void Deserialize(InputMemoryStream& stream, type& value)	\
	{	\
		stream.Read(&value);	\
	}

	ANCH_DEFINE_DEFAULT_DESERIALIZE(bool);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(char);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(int8_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(uint8_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(int16_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(uint16_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(int32_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(uint32_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(int64_t);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(uint64_t);

	ANCH_DEFINE_DEFAULT_DESERIALIZE(float);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(double);

	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector2f);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector3f);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector4f);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector2i);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector3i);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Vector4i);

	ANCH_DEFINE_DEFAULT_DESERIALIZE(Matrix2f);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Matrix3f);
	ANCH_DEFINE_DEFAULT_DESERIALIZE(Matrix4f);

	ANCH_DEFINE_DEFAULT_DESERIALIZE(SocketAddress);

	inline void Deserialize(InputMemoryStream& stream, blt::string& value)
	{
		size_t length;
		value.reserve(length + 1);
		value.clear();
		for (size_t i = 0; i < length; i++)
		{
			Deserialize(stream, value.at(i));
		}
	}

	template<typename T>
	inline void Deserialize(InputMemoryStream& stream, std::vector<T>& value)
	{
		size_t length;
		Deserialize(stream, length);
		value.reserve(length);
		value.clear();
		for (size_t i = 0; i < length; i++)
		{
			Deserialize(stream, value[i]);
		}
	}

}