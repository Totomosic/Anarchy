#pragma once
#include "Core/Events/Events.h"

namespace Anarchy
{

	struct ServerEvents
	{
	public:
		static constexpr uint32_t ClientMessageRecevied = Bolt::Events::MIN_USER_ID + 1;
	};

}