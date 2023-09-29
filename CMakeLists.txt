cmake_minimum_required(VERSION 3.16)
add_compile_definitions(QT_DISABLE_DEPRECATED_UP_TO=0x050F00)
project(knetstats)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)
set(QT_MIN_VERSION "5.15.0")

find_package (ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
        Core
        Widgets
        Network
)

find_package(KF5 REQUIRED COMPONENTS
        WidgetsAddons
)

set(CMAKE_AUTOMOC on)
set(CMAKE_AUTOUIC on)
set(CMAKE_AUTORCC on)

#qt_standard_project_setup()
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
        Qt5::Widgets
        Qt5::Network
        KF5::WidgetsAddons
)
