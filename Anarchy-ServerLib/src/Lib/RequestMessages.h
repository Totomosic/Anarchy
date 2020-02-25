#pragma once
#include "ServerLib.h"
#include "GameState.h"

#include "Lib/Entities/EntityActions.h"
#include "World/Tile.h"

namespace Anarchy
{

	// =======================================================================================
	// HEADERS
	// =======================================================================================

	using reqid_t = uint16_t;

	struct RequestHeader
	{
	public:
		reqid_t Id;
	};

	inline void Serialize(OutputMemoryStream& stream, const RequestHeader& header)
	{
		Serialize(stream, header.Id);
	}

	inline void Deserialize(InputMemoryStream& stream, RequestHeader& header)
	{
		Deserialize(stream, header.Id);
	}

	struct ResponseHeader
	{
	public:
		reqid_t RequestId;
	};

	inline void Serialize(OutputMemoryStream& stream, const ResponseHeader& header)
	{
		Serialize(stream, header.RequestId);
	}

	inline void Deserialize(InputMemoryStream& stream, ResponseHeader& header)
	{
		Deserialize(stream, header.RequestId);
	}

	// =======================================================================================
	// SERVER CONNECTION
	// =======================================================================================

	struct ServerConnectionRequest
	{
	public:
		static constexpr MessageType Type = MessageType::ConnectRequest;
	public:
		std::string Username;
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
	// CREATE CHARACTER
	// =======================================================================================

	struct CreateCharacterRequest
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterRequest;

	public:
		std::string Name;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterRequest& request)
	{
		Serialize(stream, request.Name);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterRequest& request)
	{
		Deserialize(stream, request.Name);
	}

	struct CreateCharacterResponse
	{
	public:
		static constexpr MessageType Type = MessageType::CreateCharacterResponse;
	public:
		EntityData Data;
	};

	inline void Serialize(OutputMemoryStream& stream, const CreateCharacterResponse& response)
	{
		Serialize(stream, response.Data);
	}

	inline void Deserialize(InputMemoryStream& stream, CreateCharacterResponse& response)
	{
		Deserialize(stream, response.Data);
	}

	// =======================================================================================
	// GET ENTITIES
	// =======================================================================================

	struct GetEntitiesRequest
	{
	public:
		static constexpr MessageType Type = MessageType::GetEntitiesRequest;

	public:
		int DimensionId = 0;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetEntitiesRequest& request)
	{
		Serialize(stream, request.DimensionId);
	}

	inline void Deserialize(InputMemoryStream& stream, GetEntitiesRequest& request)
	{
		Deserialize(stream, request.DimensionId);
	}

	struct GetEntitiesResponse
	{
	public:
		static constexpr MessageType Type = MessageType::GetEntitiesResponse;

	public:
		std::vector<EntityData> Entities;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetEntitiesResponse& response)
	{
		Serialize(stream, response.Entities);
	}

	inline void Deserialize(InputMemoryStream& stream, GetEntitiesResponse& response)
	{
		Deserialize(stream, response.Entities);
	}

	// =======================================================================================
	// GET TILEMAP
	// =======================================================================================

	struct GetTilemapRequest
	{
	public:
		static constexpr MessageType Type = MessageType::GetTilemapRequest;

	public:
		int DimensionId;
		int x;
		int y;
		int Width;
		int Height;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetTilemapRequest& request)
	{
		Serialize(stream, request.DimensionId);
		Serialize(stream, request.x);
		Serialize(stream, request.y);
		Serialize(stream, request.Width);
		Serialize(stream, request.Height);
	}

	inline void Deserialize(InputMemoryStream& stream, GetTilemapRequest& request)
	{
		Deserialize(stream, request.DimensionId);
		Deserialize(stream, request.x);
		Deserialize(stream, request.y);
		Deserialize(stream, request.Width);
		Deserialize(stream, request.Height);
	}

	struct GetTilemapResponse
	{
	public:
		static constexpr MessageType Type = MessageType::GetTilemapResponse;

	public:
		int x;
		int y;
		int Width;
		int Height;
		std::vector<TileType> Tiles;
	};

	inline void Serialize(OutputMemoryStream& stream, const GetTilemapResponse& response)
	{
		Serialize(stream, response.x);
		Serialize(stream, response.y);
		Serialize(stream, response.Width);
		Serialize(stream, response.Height);
		Serialize(stream, response.Tiles);
	}

	inline void Deserialize(InputMemoryStream& stream, GetTilemapResponse& response)
	{
		Deserialize(stream, response.x);
		Deserialize(stream, response.y);
		Deserialize(stream, response.Width);
		Deserialize(stream, response.Height);
		Deserialize(stream, response.Tiles);
	}

}