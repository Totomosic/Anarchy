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
		uint64_t ConnectionId;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerConnectionResponse& value)
	{
		Serialize(stream, value.Success);
		Serialize(stream, value.ConnectionId);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerConnectionResponse& value)
	{
		Deserialize(stream, value.Success);
		Deserialize(stream, value.ConnectionId);
	}

	// =======================================================================================
	// SERVER DISCONNECT
	// =======================================================================================

	struct ServerDisconnectRequest
	{
	public:
		uint64_t ConnectionId;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerDisconnectRequest& value)
	{
		Serialize(stream, value.ConnectionId);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerDisconnectRequest& value)
	{
		Deserialize(stream, value.ConnectionId);
	}

	// Used as a successful response to ServerDisconnectRequest or if the server is forcefully disconnecting someone
	struct ForceDisconnectMessage
	{
	
	};

	inline void Serialize(OutputMemoryStream& stream, const ForceDisconnectMessage& value)
	{
	
	}

	inline void Deserialize(InputMemoryStream& stream, ForceDisconnectMessage& value)
	{
	
	}

}