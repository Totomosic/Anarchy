#include "serverpch.h"
#include "Server.h"
#include "Engine/Engine.h"

namespace Anarchy
{

	void Run()
	{
		EngineCreateInfo info;
		info.UseGraphics = false;
		info.UseSockets = true;
		Engine e(info);
		e.SetApplication<Server>();
		e.Run();
	}

}

int main()
{
	Anarchy::Run();
	return 0;
}