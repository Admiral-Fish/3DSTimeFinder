lessThan(QT_MAJOR_VERSION, 5): error("You need at least Qt 5.9 to build 3DSTimeFinder")
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 9): error("You need at least Qt 5.9 to build 3DSTimeFinder")

QT += concurrent widgets
CONFIG += c++1z

TARGET = 3DSTimeFinder
TEMPLATE = app
VERSION = 1.0.0

#RC_ICONS += Images/3dstimefinder.ico
#ICON += Images/3dstimefinder.icns

DEFINES += QT_DEPRECATED_WARNINGS

RESOURCES += \
    QDarkStyleSheet/qdarkstyle/style.qrc

FORMS += \
    Forms/Controls/IVFilter.ui \
    Forms/Gen6/Event6.ui \
    Forms/Gen6/ProfileEditor6.ui \
    Forms/Gen6/ProfileManager6.ui \
    Forms/Gen6/Stationary6.ui \
    Forms/Gen7/Event7.ui \
    Forms/Gen7/ID7.ui \
    Forms/Gen7/ProfileCalibrater7.ui \
    Forms/Gen7/ProfileEditor7.ui \
    Forms/Gen7/ProfileManager7.ui \
    Forms/Gen7/Stationary7.ui \
    Forms/Gen7/Wild7.ui \
    Forms/MainWindow.ui

HEADERS += \
    Core/Gen6/EventSearcher6.hpp \
    Core/Gen6/Profile6.hpp \
    Core/Gen6/StationarySearcher6.hpp \
    Core/Gen7/EventSearcher7.hpp \
    Core/Gen7/IDSearcher7.hpp \
    Core/Gen7/Profile7.hpp \
    Core/Gen7/ProfileSearcher7.hpp \
    Core/Gen7/StationarySearcher7.hpp \
    Core/Gen7/WildSearcher7.hpp \
    Core/Global.hpp \
    Core/Parents/EventFilter.hpp \
    Core/Parents/EventResult.hpp \
    Core/Parents/Filter.hpp \
    Core/Parents/IDFilter.hpp \
    Core/Parents/IDResult.hpp \
    Core/Parents/Profile.hpp \
    Core/Parents/Result.hpp \
    Core/Parents/StationaryFilter.hpp \
    Core/Parents/StationaryResult.hpp \
    Core/Parents/WildFilter.hpp \
    Core/Parents/WildResult.hpp \
    Core/RNG/MT.hpp \
    Core/RNG/SFMT.hpp \
    Core/RNG/SHA256.hpp \
    Core/Util/Game.hpp \
    Core/Util/IDType.hpp \
    Core/Util/PIDType.hpp \
    Core/Util/Utility.hpp \
    Core/Util/WildType.hpp \
    Forms/Controls/CheckList.hpp \
    Forms/Controls/IVFilter.hpp \
    Forms/Controls/Label.hpp \
    Forms/Controls/TableView.hpp \
    Forms/Controls/TextBox.hpp \
    Forms/Gen6/Event6.hpp \
    Forms/Gen6/ProfileEditor6.hpp \
    Forms/Gen6/ProfileManager6.hpp \
    Forms/Gen6/Stationary6.hpp \
    Forms/Gen7/Event7.hpp \
    Forms/Gen7/ID7.hpp \
    Forms/Gen7/ProfileCalibrater7.hpp \
    Forms/Gen7/ProfileEditor7.hpp \
    Forms/Gen7/ProfileManager7.hpp \
    Forms/Gen7/Stationary7.hpp \
    Forms/Gen7/Wild7.hpp \
    Forms/MainWindow.hpp \
    Models/EventModel.hpp \
    Models/IDModel.hpp \
    Models/ProfileModel6.hpp \
    Models/ProfileModel7.hpp \
    Models/StationaryModel.hpp \
    Models/TableModel.hpp \
    Models/WildModel.hpp

SOURCES += \
    Core/Gen6/EventSearcher6.cpp \
    Core/Gen6/Profile6.cpp \
    Core/Gen6/StationarySearcher6.cpp \
    Core/Gen7/EventSearcher7.cpp \
    Core/Gen7/IDSearcher7.cpp \
    Core/Gen7/Profile7.cpp \
    Core/Gen7/ProfileSearcher7.cpp \
    Core/Gen7/StationarySearcher7.cpp \
    Core/Gen7/WildSearcher7.cpp \
    Core/Parents/EventFilter.cpp \
    Core/Parents/EventResult.cpp \
    Core/Parents/IDFilter.cpp \
    Core/Parents/IDResult.cpp \
    Core/Parents/Profile.cpp \
    Core/Parents/Result.cpp \
    Core/Parents/StationaryFilter.cpp \
    Core/Parents/StationaryResult.cpp \
    Core/Parents/WildFilter.cpp \
    Core/Parents/WildResult.cpp \
    Core/RNG/MT.cpp \
    Core/RNG/SFMT.cpp \
    Core/RNG/SHA256.cpp \
    Core/Util/Utility.cpp \
    Forms/Controls/CheckList.cpp \
    Forms/Controls/IVFilter.cpp \
    Forms/Controls/Label.cpp \
    Forms/Controls/TableView.cpp \
    Forms/Controls/TextBox.cpp \
    Forms/Gen6/Event6.cpp \
    Forms/Gen6/ProfileEditor6.cpp \
    Forms/Gen6/ProfileManager6.cpp \
    Forms/Gen6/Stationary6.cpp \
    Forms/Gen7/Event7.cpp \
    Forms/Gen7/ID7.cpp \
    Forms/Gen7/ProfileCalibrater7.cpp \
    Forms/Gen7/ProfileEditor7.cpp \
    Forms/Gen7/ProfileManager7.cpp \
    Forms/Gen7/Stationary7.cpp \
    Forms/Gen7/Wild7.cpp \
    Forms/MainWindow.cpp \
    Models/EventModel.cpp \
    Models/IDModel.cpp \
    Models/ProfileModel6.cpp \
    Models/ProfileModel7.cpp \
    Models/StationaryModel.cpp \
    Models/WildModel.cpp \
    main.cpp
