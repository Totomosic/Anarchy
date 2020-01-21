#pragma once
#include "../ClientListener.h"

namespace Anarchy
{

	class ConnectionManager
	{
	private:
		ClientSocket m_Socket;
		ClientListener m_Listener;

	public:
		ConnectionManager(const SocketAddress& address);

		const ClientListener& GetSocketApi() const;
		ClientListener& GetSocketApi();

		connid_t GetConnectionId() const;
		bool IsConnected() const;
		bool IsConnecting() const;

		void Update(TimeDelta delta);

	};

}