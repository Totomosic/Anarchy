#pragma once
#include "ServerLib.h"

namespace Anarchy
{

	// =======================================================================================
	// SERVER CONNECTION
	// =======================================================================================

	struct ServerConnectionRequest
	{
	public:
		blt::string Username;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerConnectionRequest& value)
	{
		Serialize(stream, value.Username);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerConnectionRequest& value)
	{
		Deserialize(stream, value.Username);
	}

	struct ServerConnectionResponse
	{
	public:
		bool Success;
		uint64_t PlayerId;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerConnectionResponse& value)
	{
		Serialize(stream, value.Success);
		Serialize(stream, value.PlayerId);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerConnectionResponse& value)
	{
		Deserialize(stream, value.Success);
		Deserialize(stream, value.PlayerId);
	}

}