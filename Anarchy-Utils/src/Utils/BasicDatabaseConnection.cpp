#include "BasicDatabaseConnection.h"
#include "Core/Utils/Filesystem/Filesystem.h"
#include <filesystem>

namespace Anarchy
{

	BasicDatabaseConnection::BasicDatabaseConnection(const DirectoryPath& databaseDirectory)
		: m_DatabaseDirectory(databaseDirectory)
	{
	}

	void BasicDatabaseConnection::Initialize()
	{
		std::filesystem::create_directory(m_DatabaseDirectory.Path().c_str());
		Filesystem::Open(m_DatabaseDirectory.Path() + "Users.txt", OpenMode::Write);
		Filesystem::Open(m_DatabaseDirectory.Path() + "Inventories.txt", OpenMode::Write);
	}

	void BasicDatabaseConnection::Destroy()
	{
		std::filesystem::remove_all(m_DatabaseDirectory.Path().c_str());
	}

}