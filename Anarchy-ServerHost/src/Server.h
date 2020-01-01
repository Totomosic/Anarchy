#pragma once
#include "serverpch.h"
#include "ServerSocket.h"

namespace Anarchy
{

	class Server : public Application
	{
	private:

	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;

	};

}