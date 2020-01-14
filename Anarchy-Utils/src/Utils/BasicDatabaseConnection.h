#pragma once
#include "Utils.h"
#include "Core/Utils/Filesystem/Directorypath.h"

namespace Anarchy
{

	class BasicDatabaseConnection
	{
	private:
		DirectoryPath m_DatabaseDirectory;

	public:
		BasicDatabaseConnection(const DirectoryPath& databaseDirectory);

		void Initialize();
		void Destroy();

	};

}