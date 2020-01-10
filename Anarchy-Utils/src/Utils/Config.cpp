#include "Config.h"
#include "Core/Utils/Filesystem/Filesystem.h"

namespace Anarchy
{

	std::unordered_map<blt::string, blt::string> LoadConfigFile(const blt::string& filepath)
	{
		File f = Filesystem::Open(filepath, OpenMode::Read);
		blt::string data = f.ReadText();
		std::vector<blt::string> lines = data.split('\n');

		std::unordered_map<blt::string, blt::string> result;

		for (blt::string& line : lines)
		{
			line.remove_all(" \t\r\n");
			uint32_t equals = line.find('=');
			if (equals != blt::string::npos)
			{
				result[line.substr(0, equals)] = line.substr(equals + 1);
			}
		}
		return result;
	}

	SocketAddress LoadServerConfig(const blt::string& filepath)
	{
		auto config = LoadConfigFile(filepath);
		return SocketAddress(config["Address"], config["Port"]);
	}

}