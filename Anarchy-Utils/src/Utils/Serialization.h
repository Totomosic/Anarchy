#pragma once
#include "Utils.h"

namespace Anarchy
{

#define ANCH_DEFINE_DEFAULT_SERIALIZE(type)	\
	inline void Serialize(OutputMemoryStream& stream, const type& value)	\
	{	\
		stream.Write(value);	\
	}

	ANCH_DEFINE_DEFAULT_SERIALIZE(bool);
	ANCH_DEFINE_DEFAULT_SERIALIZE(char);
	ANCH_DEFINE_DEFAULT_SERIALIZE(int8_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(uint8_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(int16_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(uint16_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(int32_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(uint32_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(int64_t);
	ANCH_DEFINE_DEFAULT_SERIALIZE(uint64_t);

	ANCH_DEFINE_DEFAULT_SERIALIZE(float);
	ANCH_DEFINE_DEFAULT_SERIALIZE(double);

	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector2f);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector3f);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector4f);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector2i);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector3i);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Vector4i);

	ANCH_DEFINE_DEFAULT_SERIALIZE(Matrix2f);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Matrix3f);
	ANCH_DEFINE_DEFAULT_SERIALIZE(Matrix4f);
	
	ANCH_DEFINE_DEFAULT_SERIALIZE(SocketAddress);

	inline void Serialize(OutputMemoryStream& stream, const std::string& value)
	{
		Serialize(stream, (size_t)value.size());
		for (char c : value)
		{
			Serialize(stream, c);
		}
	}

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const std::vector<T>& value)
	{
		Serialize(stream, (size_t)value.size());
		for (const T& v : value)
		{
			Serialize(stream, v);
		}
	}

	template<typename T>
	inline void Serialize(OutputMemoryStream& stream, const std::optional<T>& value)
	{
		Serialize(stream, value.has_value());
		if (value.has_value())
		{
			Serialize(stream, value.value());
		}
	}

}