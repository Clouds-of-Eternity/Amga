project "cute_server"
    kind "ConsoleApp"
    language "C++"
    rtti "Off"
    cppdialect "C++11"
    exceptionhandling "Off"
    staticruntime "Off"
    targetdir "bin/%{cfg.buildcfg}"
    warnings "off"

    includedirs {
        "./",
        "../dependencies/Astral.Core"
    }
    files {
        "**.cpp",
        "**.c"
    }