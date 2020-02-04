#pragma once
#include "serverpch.h"
#include "Engine/Application.h"
#include "Lib/Entities/ActionBuffer.h"

namespace Anarchy
{

	extern SocketAddress ServerAddress;

	class Server : public Application
	{
	private:
		ActionBuffer m_Actions;

	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;

	};

}