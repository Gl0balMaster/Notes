cmake_minimum_required(VERSION 3.16)

project(task VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ищем Qt (поддерживаем Qt5 и Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# Проверяем наличие SQLite
find_package(SQLite3 REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.h mainwindow.cpp
    note.h
    ExpandableNoteButton.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(task
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        notebtn.cpp
    )
else()
    if(ANDROID)
        add_library(task SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(task
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Подключаем Qt-модули и SQLite
target_link_libraries(task PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
)

# Настройки для macOS и Windows
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.task)
endif()

set_target_properties(task PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Установка
include(GNUInstallDirs)
install(TARGETS task
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(task)
endif()
