#pragma once
#include "Utils.h"

namespace Anarchy
{

	SocketAddress LoadServerConfig(const blt::string& filepath = "./Config/ServerHostInfo.cfg");

}