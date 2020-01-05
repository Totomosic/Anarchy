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
		bool Success;
		connid_t ConnectionId;
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
		static constexpr MessageType Type = MessageType::DisconnectRequest;
	public:
		connid_t ConnectionId;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerDisconnectRequest& value)
	{
		Serialize(stream, value.ConnectionId);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerDisconnectRequest& value)
	{
		Deserialize(stream, value.ConnectionId);
	}

	struct ServerDisconnectResponse
	{
	public:
		static constexpr MessageType Type = MessageType::DisconnectResponse;
	public:
		bool Success;
	};

	inline void Serialize(OutputMemoryStream& stream, const ServerDisconnectResponse& value)
	{
		Serialize(stream, value.Success);
	}

	inline void Deserialize(InputMemoryStream& stream, ServerDisconnectResponse& value)
	{
		Deserialize(stream, value.Success);
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