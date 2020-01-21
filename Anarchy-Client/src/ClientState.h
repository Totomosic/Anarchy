#pragma 
#include "Networking/Connection/ConnectionManager.h"
#include "Entities/ClientEntityCollection.h"

namespace Anarchy
{

	class ClientState
	{
	private:
		static std::unique_ptr<ClientState> s_Instance;

	private:
		std::unique_ptr<ConnectionManager> m_Connection;
		std::unique_ptr<ClientEntityCollection> m_Entities;

	public:
		static ClientState& Get();
		static void Terminate();
		
	public:
		ClientState();

		void InitializeConnection(const SocketAddress& serverAddress);
		void InitializeEntities(Scene& gameScene, Layer& gameLayer);

		bool HasConnection() const;
		const ConnectionManager& GetConnection() const;
		ConnectionManager& GetConnection();
		void CloseConnection();

		bool HasEntities() const;
		const ClientEntityCollection& GetEntities() const;
		ClientEntityCollection& GetEntities();
		void DestroyEntities();

	};

}