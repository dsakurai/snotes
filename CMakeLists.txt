cmake_minimum_required(VERSION 3.27)
project(SimpleMarkdown)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)
        
include(FetchContent)

FetchContent_Declare(
    qmarkdowntext_project
    GIT_REPOSITORY https://github.com/pbek/qmarkdowntextedit.git
    GIT_TAG b1cb6a7d20d2849f31f37822ef24e033c9d7c044
)
FetchContent_MakeAvailable(qmarkdowntext_project)
FetchContent_GetProperties(qmarkdowntext_project) # Expose paths as variables


add_executable(simplemd
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        ProjectFolderModel.h
        NotesListView.h
)

target_include_directories(simplemd
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(simplemd PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_include_directories(simplemd
        PRIVATE
        "${qmarkdowntext_project_SOURCE_DIR}" # The header of the QWidget for markdown. The problem is that it's in the build directory...
)

target_link_libraries(simplemd
        Qt::Core
        Qt::Gui
        Qt::Widgets
        qmarkdowntextedit
)

