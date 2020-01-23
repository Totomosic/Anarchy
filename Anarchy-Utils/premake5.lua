do
    local ProjectName = "Anarchy-Utils"
    local BuildDir = "../bin/" .. outputdir .. "/" .. ProjectName
    local ObjDir = "../bin-int/" .. outputdir .. "/" .. ProjectName

    -- Path to Bolt install dir from project dir
    local BoltInstallDir = "../Bolt/"

    project (ProjectName)
        location ""
        kind "StaticLib"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"
        
        targetdir (BuildDir)
        objdir (ObjDir)
        
        files
        {
            "src/**.h",
            "src/**.hpp",
            "src/**.cpp"
        }
        
        includedirs
        {
            BoltInstallDir .. "%{IncludeDirs.GLFW}",
            BoltInstallDir .. "%{IncludeDirs.Glad}",
            BoltInstallDir .. "%{IncludeDirs.ImGui}",
            BoltInstallDir .. "%{IncludeDirs.spdlog}",
            BoltInstallDir .. "%{IncludeDirs.FreeTypeGL}",
            BoltInstallDir .. "%{IncludeDirs.FreeType}",
            BoltInstallDir .. "%{IncludeDirs.Lua}",
            BoltInstallDir .. "%{IncludeDirs.BoltLib}",
            BoltInstallDir .. "%{IncludeDirs.Bolt}",
            "src",
            "vendor"
        }

        links
        {
            "Bolt-Core"
        }

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_WINDOWS",
                "BLT_BUILD_STATIC",
                "_CRT_SECURE_NO_WARNINGS"
            }

        filter "system:linux"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_LINUX",
                "BLT_BUILD_STATIC"
            }

        filter "system:macosx"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_MAC",
                "BLT_BUILD_STATIC"
            }

        filter "configurations:Debug"
            defines "BLT_DEBUG"
            runtime "Debug"
            symbols "on"

        filter "configurations:Release"
            defines "BLT_RELEASE"
            runtime "Release"
            optimize "on"

        filter "configurations:Dist"
            defines "BLT_DIST"
            runtime "Release"
            optimize "on"
end