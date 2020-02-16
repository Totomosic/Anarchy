#include "clientpch.h"
#include "ConnectionManager.h"

#include "ClientState.h"
#include "Events.h"

namespace Anarchy
{

	ConnectionManager::ConnectionManager(const SocketAddress& address)
		: m_Socket(address), m_Listener(&m_Socket)
	{
		
	}

	const ClientListener& ConnectionManager::GetSocketApi() const
	{
		return m_Listener;
	}

	ClientListener& ConnectionManager::GetSocketApi()
	{
		return m_Listener;
	}

	connid_t ConnectionManager::GetConnectionId() const
	{
		return m_Listener.GetConnectionId();
	}

	bool ConnectionManager::IsConnected() const
	{
		return m_Listener.IsConnected();
	}

	bool ConnectionManager::IsConnecting() const
	{
		return m_Listener.IsConnecting();
	}

	void ConnectionManager::Update(TimeDelta delta)
	{
		m_Listener.Update(delta);
	}

}