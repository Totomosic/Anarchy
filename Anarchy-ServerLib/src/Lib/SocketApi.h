#pragma once
#include "Authentication.h"
#include "GameMessages.h"
#include "Core/Tasks/Task.h"

namespace Anarchy
{

	template<typename T>
	struct ServerRequest
	{
	public:
		SocketAddress From;
		T Request;
	};

	class ServerSocketApi
	{
	public:
		virtual ServerConnectionResponse Connect(const ServerRequest<ServerConnectionRequest>& request) = 0;
		virtual ServerDisconnectResponse Disconnect(const ServerRequest<ServerDisconnectRequest>& request) = 0;

		virtual CreateCharacterResponse CreateCharacter(const ServerRequest<CreateCharacterRequest>& request) = 0;
		virtual GetEntitiesResponse GetEntities(const ServerRequest<GetEntitiesRequest>& request) = 0;
	};

	class ClientSocketApi
	{
	public:
		template<typename T>
		using Promise = Task<std::optional<T>>;

	public:
		virtual Promise<ServerConnectionResponse> Connect(const ServerConnectionRequest& request, double timeoutSeconds) = 0;
		virtual Promise<ServerDisconnectResponse> Disconnect(const ServerDisconnectRequest& request, double timeoutSeconds) = 0;

		virtual Promise<CreateCharacterResponse> CreateCharacter(const CreateCharacterRequest& request, double timeoutSeconds) = 0;
		virtual Promise<GetEntitiesResponse> GetEntities(const GetEntitiesRequest& request, double timeoutSeconds) = 0;
	};

}