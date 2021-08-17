project(3DSTimeFinderCore)

include_directories("${CMAKE_SOURCE_DIR}/Externals")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/Zc:inline)
endif ()

if (UNIX)
    include(GetTargetArch)
    get_target_arch(ARCH)
    if ((ARCH STREQUAL "x86_64") OR (ARCH STREQUAL "i686"))
        add_compile_options(-msse4.1)
    elseif (ARCH STREQUAL "arm")
        add_compile_options(-mfpu=neon)
    endif ()
endif ()

add_library(3DSTimeFinderCore STATIC
    Gen6/EventSearcher6.cpp
    Gen6/Profile6.cpp
    Gen6/StationarySearcher6.cpp
    Gen7/EventSearcher7.cpp
    Gen7/IDSearcher7.cpp
    Gen7/Profile7.cpp
    Gen7/ProfileSearcher7.cpp
    Gen7/StationarySearcher7.cpp
    Gen7/WildSearcher7.cpp
    Parents/EventFilter.cpp
    Parents/IDFilter.cpp
    Parents/IDResult.cpp
    Parents/Profile.cpp
    Parents/ProfileLoader.cpp
    Parents/Result.cpp
    Parents/StationaryFilter.cpp
    Parents/WildFilter.cpp
    RNG/MT.cpp
    RNG/SFMT.cpp
    RNG/SHA256.cpp
    Util/DateTime.cpp
    Util/Utility.cpp
)
