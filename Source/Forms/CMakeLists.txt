project(3DSTimeFinder VERSION 1.0.0 LANGUAGES CXX)
add_compile_definitions(VERSION="1.0.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
	
set(PROJECT_QRC
    "${CMAKE_SOURCE_DIR}/Externals/QDarkStyleSheet/qdarkstyle/dark/style.qrc"
)

set(PROJECT_UI	
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

target_link_libraries(3DSTimeFinder PRIVATE 3DSTimeFinderCore Qt5::Core Qt5::Widgets)