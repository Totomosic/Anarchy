#pragma once
#include "serverpch.h"
#include "Engine/Application.h"
#include "Lib/Entities/ActionQueue.h"

namespace Anarchy
{

	extern SocketAddress ServerAddress;

	class Server : public Application
	{
	private:
		ActionQueue m_Actions;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_FrameStart;
		double m_AverageDelta;

	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;

	};

}