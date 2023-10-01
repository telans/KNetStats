cmake_minimum_required(VERSION 3.16)
project(knetstats)

if(NOT CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    MESSAGE(FATAL_ERROR "Building KNetStats is not supported on ${CMAKE_HOST_SYSTEM_NAME}")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(QT_MIN_VERSION "5.5.0")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
        Core
        Network
        Widgets
)
find_package(KF5 REQUIRED COMPONENTS WidgetsAddons)

set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)
set(CMAKE_AUTORCC on)

add_executable(knetstats
        src/chart.cpp
        src/configure.cpp
        src/knetstats.cpp
        src/knetstatsview.cpp
        src/main.cpp
        src/statistics.cpp
        src/configurebase.ui
        src/statisticsbase.ui
        res/knetstats.qrc
)

target_link_libraries(knetstats PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Widgets
        KF5::WidgetsAddons
)

install(TARGETS knetstats RUNTIME)
install(FILES dist/com.telans.KNetStats.desktop
        DESTINATION share/applications)
install(FILES dist/com.telans.KNetStats.appdata.xml
        DESTINATION share/metainfo)
install(DIRECTORY dist/hicolor
        DESTINATION share/icons)
