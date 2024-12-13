workspace "EternityWorks"
    configurations { "Debug", "Release" }
    defines "ETERNITY_DOTNET"

    filter "system:windows"
        defines { "WINDOWS", "_WIN32" }
        architecture "x86_64"

    filter "system:linux"
        defines { "LINUX", "POSIX" }

    filter "system:macosx"
        defines { "MACOS", "POSIX" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"

    include("src/cute_client")
    include("src/cute_server")