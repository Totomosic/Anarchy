do
    local ProjectName = "Anarchy-Client"
    local BuildDir = "../bin/" .. outputdir .. "/" .. ProjectName
    local ObjDir = "../bin-int/" .. outputdir .. "/" .. ProjectName

    -- Path to Bolt install dir from project dir
    local BoltInstallDir = "../Bolt/"

    project (ProjectName)
        location ""
        kind "ConsoleApp"
        language "C++"
        cppdialect "C++17"
        staticruntime "on"

        pchheader ("clientpch.h")
        pchsource ("src/clientpch.cpp")
        
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
            BoltInstallDir .. "%{IncludeDirs.Python}",
            BoltInstallDir .. "%{IncludeDirs.Bolt}",
            "../%{IncludeDirs.AnarchyUtils}",
            "../%{IncludeDirs.AnarchyServerLib}",
            "src"
        }

        links
        {
            "Anarchy-ServerLib",
        }

        filter "system:windows"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_WINDOWS",
                "BLT_BUILD_STATIC",
                "_CRT_SECURE_NO_WARNINGS",
            }

        filter "system:linux"
            systemversion "latest"

            defines
            {
                "BLT_PLATFORM_LINUX",
                "BLT_BUILD_STATIC",
            }

            links
            {
                "Anarchy-Utils",
                "Bolt-Core",
                "GLFW",
                "Glad",
                "ImGui",
                "Lua",
                "FreeType-GL",
                "FreeType",                          
                "stdc++fs",
                "dl",
                "X11",
                "pthread"
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