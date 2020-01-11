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
		static constexpr MessageType Type = MessageType::ConnectRequest;
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
		static constexpr MessageType Type = MessageType::ConnectResponse;
	public:
		connid_t ConnectionId = 0;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerConnectionResponse& value)
	{
		Serialize(stream, value.ConnectionId);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerConnectionResponse& value)
	{
		Deserialize(stream, value.ConnectionId);
	}

	// =======================================================================================
	// SERVER DISCONNECT
	// =======================================================================================

	struct ServerDisconnectRequest
	{
	public:
		static constexpr MessageType Type = MessageType::DisconnectRequest;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerDisconnectRequest& value)
	{
	}

	inline void Deserialize(InputMemoryStream& stream, ServerDisconnectRequest& value)
	{
	}

	struct ServerDisconnectResponse
	{
	public:
		static constexpr MessageType Type = MessageType::DisconnectResponse;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerDisconnectResponse& value)
	{
	}

	inline void Deserialize(InputMemoryStream& stream, ServerDisconnectResponse& value)
	{
	}

	// =======================================================================================
	// FORCE DISCONNECT
	// =======================================================================================
	
	struct ForceDisconnectMessage
	{
	public:
		static constexpr MessageType Type = MessageType::ForceDisconnect;
	};

	inline void Serialize(OutputMemoryStream& stream, const ForceDisconnectMessage& value)
	{
	
	}

	inline void Deserialize(InputMemoryStream& stream, ForceDisconnectMessage& value)
	{
	
	}

}