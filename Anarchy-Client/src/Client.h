#pragma once
#include "clientpch.h"

namespace Anarchy
{

	class Client : public Application
	{
	private:

	public:
		void Init() override;
		void Tick() override;
		void Update() override;
		void Render() override;

	};

}