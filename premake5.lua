workspace "Anarchy"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

-- Path to directory containing Bolt-Core
BoltDir = "Bolt/"

include (BoltDir .. "BoltInclude.lua")

IncludeDirs["AnarchyUtils"] = "Anarchy-Utils/src"
IncludeDirs["AnarchyServerLib"] = "Anarchy-ServerLib/src"
IncludeDirs["AnarchyServerHost"] = "Anarchy-ServerHost/src"
IncludeDirs["AnarchyClient"] = "Anarchy-Client/src"

group ("Anarchy")
include ("Anarchy-Utils")
include ("Anarchy-ServerLib")
include ("Anarchy-ServerHost")
include ("Anarchy-Client")
group ("Tools")
include ("Tools/WorldEditor")