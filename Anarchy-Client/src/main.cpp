#include "clientpch.h"
#include "Client.h"

namespace Anarchy
{

	void Run()
	{
		EngineCreateInfo info;
		info.UseGraphics = true;
		info.UseSockets = true;
		info.WindowInfo.Title = "Anarchy";
		info.WindowInfo.Width = 1280;
		info.WindowInfo.Height = 720;
		Engine e(info);
		e.SetApplication<Client>();
		e.Run();
	}

}

int main()
{
	Anarchy::Run();
	return 0;
}