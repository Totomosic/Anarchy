#pragma once
#include "Utils.h"

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