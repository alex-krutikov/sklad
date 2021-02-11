cmake_minimum_required(VERSION 3.7)

project(sklad)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

add_executable(app WIN32)

set_target_properties(app PROPERTIES OUTPUT_NAME "sklad")

target_link_libraries(app Qt5::Network)
target_link_libraries(app Qt5::PrintSupport)
target_link_libraries(app Qt5::Sql)
target_link_libraries(app Qt5::Widgets)

target_include_directories(app
    PRIVATE
        .
)

target_sources(app
    PRIVATE
        bom.cpp
        bom.h
        dialogs.cpp
        dialogs.h
        dialogs2.cpp
        dialogs2.h
        main.cpp
        main.h
        mainwindow.cpp
        mainwindow.h
        misc.cpp
        misc.h
        price.cpp
        price.h
        proektform.cpp
        proektform.h
        sklad.rc
        sqlactions.cpp
        sqlactions.h
        trebnaklad.cpp
        trebnaklad.h
        users.cpp
        users.h
)

install(TARGETS app DESTINATION .)
