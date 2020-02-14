#pragma once
#include "Authentication.h"
#include "GameMessages.h"
#include "Entities/EntityActions.h"
#include "Core/Tasks/Task.h"

namespace Anarchy
{

	template<typename T>
	struct ServerRequest
	{
	public:
		SocketAddress From = {};
		T Request = {};
	};

	class ServerSocketApi
	{
	public:
		virtual void OnKeepAlive(const ServerNetworkMessage<KeepAlivePacket>& packet) = 0;
		virtual void SendKeepAlive(const std::vector<connid_t>& connections) = 0;
		virtual void ForceDisconnectConnections(const std::vector<connid_t>& connectionIds) = 0;

		virtual std::optional<ServerConnectionResponse> Connect(const ServerRequest<ServerNetworkMessage<ServerConnectionRequest>>& request) = 0;
		virtual std::optional<ServerDisconnectResponse> Disconnect(const ServerRequest<ServerNetworkMessage<ServerDisconnectRequest>>& request) = 0;

		virtual std::optional<CreateCharacterResponse> CreateCharacter(const ServerRequest<ServerNetworkMessage<CreateCharacterRequest>>& request) = 0;
		virtual std::optional<GetEntitiesResponse> GetEntities(const ServerRequest<ServerNetworkMessage<GetEntitiesRequest>>& request) = 0;
		virtual std::optional<GetTilemapResponse> GetTilemap(const ServerRequest<ServerNetworkMessage<GetTilemapRequest>>& request) = 0;

		virtual void SpawnEntities(const std::vector<connid_t>& connections, const SpawnEntitiesRequest& request, connid_t ownerConnectionId) = 0;
		virtual void DestroyEntities(const std::vector<connid_t>& connections, const DestroyEntitiesRequest& request) = 0;
		virtual void UpdateEntities(const std::vector<connid_t>& connections, const UpdateEntitiesRequest& request) = 0;

		virtual void OnAction(const ServerNetworkMessage<GenericAction>& command) = 0;
	};

	class ClientSocketApi
	{
	public:
		template<typename T>
		using Promise = Task<std::optional<T>>;

	public:
		virtual void OnKeepAlive(const NetworkMessage<KeepAlivePacket>& packet) = 0;
		virtual void SendKeepAlive() = 0;

		virtual Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds) = 0;
		virtual Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds) = 0;

		virtual Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds) = 0;
		virtual Promise<GetEntitiesResponse> GetEntities(const GetEntitiesRequest& request, double timeoutSeconds) = 0;
		virtual Promise<GetTilemapResponse> GetTilemap(const GetTilemapRequest& request, double timeoutSeconds) = 0;

		virtual void SpawnEntities(const NetworkMessage<SpawnEntitiesRequest>& request) = 0;
		virtual void DestroyEntities(const NetworkMessage<DestroyEntitiesRequest>& request) = 0;
		virtual void UpdateEntities(const NetworkMessage<UpdateEntitiesRequest>& request) = 0;

		virtual void SendAction(const GenericAction& command) = 0;
	};

}