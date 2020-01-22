#pragma once
#include "clientpch.h"
#include "Lib/Entities/ActionBuffer.h"

namespace Anarchy
{

	class Client : public Application
	{
	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;
		void Exit() override;

	};

}