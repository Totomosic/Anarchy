#include "Config.h"
#include "Core/Utils/Filesystem/Filesystem.h"

namespace Anarchy
{

	std::unordered_map<std::string, std::string> LoadConfigFile(const std::string& filepath)
	{
		File f = Filesystem::Open(filepath, OpenMode::Read);
		std::string data = f.ReadText();
		std::vector<std::string> lines = blt::split(data, '\n');

		std::unordered_map<std::string, std::string> result;

		for (std::string& line : lines)
		{
			blt::remove_all(line, " \t\r\n");
			uint32_t equals = line.find('=');
			if (equals != std::string::npos)
			{
				result[line.substr(0, equals)] = line.substr(equals + 1);
			}
		}
		return result;
	}

	SocketAddress LoadServerConfig(const std::string& filepath)
	{
		auto config = LoadConfigFile(filepath);
		return SocketAddress(config["Address"], config["Port"]);
	}

}