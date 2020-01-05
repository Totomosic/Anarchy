#pragma once
#include "Utils.h"
#include "Utils/Serialization.h"
#include "Utils/Deserialization.h"
#include "Utils/DatabaseConnection.h"
#include "Lib/MessageType.h"

namespace Anarchy
{

	using prefab_t = int;
	using entityid_t = uint32_t;
	using connid_t = uint16_t;
	static constexpr connid_t InvalidConnectionId = (connid_t)-1;

}