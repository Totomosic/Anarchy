workspace "Anarchy"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

-- Path to directory containing Bolt-Core
BoltDir = "Bolt/"

include (BoltDir .. "BoltInclude.lua")

group ("Anarchy")
include ("Anarchy-Utils")