project(3DSTimeFinder VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BRANCH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions("-DGIT_BRANCH=${GIT_BRANCH}")
add_definitions("-DGIT_COMMIT=${GIT_COMMIT}")

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
    ${CMAKE_BINARY_DIR}/generated/version.h
)

include_directories(${CMAKE_BINARY_DIR}/generated)

set(PROJECT_QRC
    "${CMAKE_SOURCE_DIR}/Externals/QDarkStyleSheet/qdarkstyle/dark/darkstyle.qrc"
)

set(PROJECT_UI
    Controls/Filter.ui
    Controls/IVFilter.ui
    Gen6/Event6.ui
    Gen6/ProfileEditor6.ui
    Gen6/ProfileManager6.ui
    Gen6/Stationary6.ui
    Gen7/Event7.ui
    Gen7/ID7.ui
    Gen7/ProfileCalibrater7.ui
    Gen7/ProfileEditor7.ui
    Gen7/ProfileManager7.ui
    Gen7/Stationary7.ui
    Gen7/Wild7.ui
    MainWindow.ui
)

set(PROJECT_SOURCES
    Controls/CheckList.cpp
    Controls/ComboBox.cpp
    Controls/DateTimeEdit.cpp
    Controls/Filter.cpp
    Controls/IVFilter.cpp
    Controls/Label.cpp
    Controls/TableView.cpp
    Controls/TextBox.cpp
    Gen6/Event6.cpp
    Gen6/ProfileEditor6.cpp
    Gen6/ProfileManager6.cpp
    Gen6/Stationary6.cpp
    Gen7/Event7.cpp
    Gen7/ID7.cpp
    Gen7/ProfileCalibrater7.cpp
    Gen7/ProfileEditor7.cpp
    Gen7/ProfileManager7.cpp
    Gen7/Stationary7.cpp
    Gen7/Wild7.cpp
    MainWindow.cpp
    Models/EventModel.cpp
    Models/IDModel.cpp
    Models/ProfileModel6.cpp
    Models/ProfileModel7.cpp
    Models/StationaryModel.cpp
    Models/WildModel.cpp
    main.cpp
)

if (WIN32)
    add_executable(3DSTimeFinder
        WIN32
        ${PROJECT_QRC}
        ${PROJECT_UI}
        ${PROJECT_SOURCES}
    )
elseif (APPLE)
    add_executable(3DSTimeFinder
        MACOSX_BUNDLE
        ${PROJECT_QRC}
        ${PROJECT_UI}
        ${PROJECT_SOURCES}
    )
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    add_executable(3DSTimeFinder
        ${PROJECT_QRC}
        ${PROJECT_UI}
        ${PROJECT_SOURCES}
    )
endif ()

target_link_libraries(3DSTimeFinder PRIVATE 3DSTimeFinderCore Qt::Core Qt::Widgets)
