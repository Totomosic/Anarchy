#pragma once
#include "serverpch.h"
#include "Engine/Application.h"
#include "Lib/Entities/CommandBuffer.h"

namespace Anarchy
{

	class Server : public Application
	{
	private:
		CommandBuffer m_Commands;

	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;

	};

}