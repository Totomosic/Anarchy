#include "serverpch.h"
#include "Server.h"
#include "Engine/Engine.h"
#include "../vendor/argparse/argparse.h"

namespace Anarchy
{

	SocketAddress ServerAddress;
	Bolt::DirectoryPath WorldDirectory;

	void Run(int argc, const char** argv)
	{
		EngineCreateInfo info;
		info.UseGraphics = false;
		info.UseSockets = true;
		Engine e(info);

		argparse::ArgumentParser parser("Anarchy-ServerHost");
		parser.add_argument()
			.name("--bind-address")
			.description("Address to bind to")
			.required(true);
		parser.add_argument()
			.names({ "-p", "--port" })
			.description("Port to bind to")
			.required(true);
		parser.add_argument()
			.names({ "-w", "--world" })
			.description("Path to the directory containing the world")
			.required(true);
		parser.enable_help();
		auto error = parser.parse(argc, argv);
		if (error)
		{
			BLT_FATAL(error);
			return;
		}
		if (parser.exists("help"))
		{
			parser.print_help();
			return;
		}

		std::string address = parser.get<std::string>("bind-address");
		int port = parser.get<int>("port");

		ServerAddress = SocketAddress(address, port);
		WorldDirectory = Bolt::DirectoryPath(parser.get<std::string>("world"));

		e.SetApplication<Server>();
		e.Run();
	}

}

int main(int argc, const char** argv)
{
	Anarchy::Run(argc, argv);
	return 0;
}