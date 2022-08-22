cmake_minimum_required(VERSION 3.7)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

project(sklad)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Network REQUIRED)
find_package(Qt5 COMPONENTS Sql REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)


add_library(mysql_plugin STATIC)

target_compile_definitions(mysql_plugin 
    PRIVATE
        QT_PLUGIN
        QT_STATICPLUGIN
)

target_include_directories(mysql_plugin
    PRIVATE
        $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/sql/mysql/include>
        $<$<PLATFORM_ID:Linux>:/usr/include/mysql>
        ${Qt5Sql_PRIVATE_INCLUDE_DIRS}
)

target_sources(mysql_plugin
    PRIVATE
        sql/qsql_mysql.cpp
        sql/qsql_mysql_p.h
        sql/main.cpp
)

target_link_libraries(mysql_plugin
    Qt5::Sql
    $<$<PLATFORM_ID:Windows>:${CMAKE_CURRENT_SOURCE_DIR}/sql/mysql/lib/libmysql.lib>
    $<$<PLATFORM_ID:Linux>:libmysqlclient.so>
)

add_executable(app WIN32)

set_target_properties(app PROPERTIES OUTPUT_NAME "sklad")

target_link_libraries(app Qt5::Network)
target_link_libraries(app Qt5::PrintSupport)
target_link_libraries(app Qt5::Sql)
target_link_libraries(app Qt5::Widgets)
target_link_libraries(app mysql_plugin)

target_include_directories(app
    PRIVATE
        .
)

target_sources(app
    PRIVATE
        analyzer.cpp
        analyzer.h
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
        sklad.qrc
        sqlactions.cpp
        sqlactions.h
        trebnaklad.cpp
        trebnaklad.h
        users.cpp
        users.h
)


install(TARGETS app DESTINATION .)
