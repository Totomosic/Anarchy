#pragma once
#include "ServerSocket.h"
#include "Lib/SocketApi.h"

namespace Anarchy
{

	class ServerListener : public ServerSocketApi
	{
	private:
		ScopedEventListener m_Listener;
		ServerSocket& m_ServerSocket;

		std::unordered_map<MessageType, std::function<void(const SocketAddress&, InputMemoryStream&)>> m_MessageHandlers;

	public:
		ServerListener(ServerSocket& socket);

		template<typename TResponse, typename TRequest>
		void Register(const std::function<TResponse(const ServerRequest<TRequest>&)>& callback)
		{
			MessageType messageType = TRequest::Type;
			BLT_ASSERT(m_MessageHandlers.find(messageType) == m_MessageHandlers.end(), "Handler already exists for message type");
			m_MessageHandlers[messageType] = [callback, this](const SocketAddress& address, InputMemoryStream& data)
			{
				ServerRequest<TRequest> srequest;
				srequest.From = address;
				Deserialize(data, srequest.Request);
				TResponse response = callback(srequest);
				m_ServerSocket.SendPacket(address, TResponse::Type, response);
			};
		}

		template<typename TRequest>
		void Register(const std::function<void(const ServerRequest<TRequest>&)>& callback)
		{
			MessageType messageType = TRequest::Type;
			BLT_ASSERT(m_MessageHandlers.find(messageType) == m_MessageHandlers.end(), "Handler already exists for message type");
			m_MessageHandlers[messageType] = [callback](const SocketAddress& address, InputMemoryStream& data)
			{
				ServerRequest<TRequest> srequest;
				srequest.From = address;
				Deserialize(data, srequest.Request);
				callback(srequest);
			};
		}

		ServerConnectionResponse Connect(const ServerRequest<ServerConnectionRequest>& request) override;
		ServerDisconnectResponse Disconnect(const ServerRequest<ServerDisconnectRequest>& request) override;

		CreateCharacterResponse CreateCharacter(const ServerRequest<CreateCharacterRequest>& request) override;
		GetEntitiesResponse GetEntities(const ServerRequest<GetEntitiesRequest>& request) override;
	};

}