cmake_minimum_required(VERSION 3.27)
project(SimpleMarkdown LANGUAGES CXX)

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

FetchContent_Declare(
        rapidyaml
        GIT_REPOSITORY https://github.com/biojppm/rapidyaml.git
        GIT_TAG v0.5.0
)
FetchContent_GetProperties(rapidyaml)
if(NOT rapidyaml_POPULATED)
    message(STATUS "Fetching rapid yaml...")
    FetchContent_Populate(rapidyaml)
    add_subdirectory(${rapidyaml_SOURCE_DIR} ${rapidyaml_BINARY_DIR})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED yes)
set(CMAKE_CXX_EXTENSIONS off)

add_executable(simplenotes
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        NotesFolderModel.h
        NotesFolderModel.cpp
        NotesListView.h
        Defaults.h
        
        resources/resources.qrc
        About_this_app.h
        Settings.h
        SNotesMarkdownEditor.cpp
        SNotesMarkdownEditor.h
        NoteViewsArea.h
        NotesListView.cpp
)

set_target_properties(simplenotes PROPERTIES CXX_SCAN_FOR_MODULES OFF)

target_include_directories(simplenotes
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

set_target_properties(simplenotes PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

target_include_directories(simplenotes
        PRIVATE
        "${qmarkdowntext_project_SOURCE_DIR}" # The header of the QWidget for markdown. The problem is that it's in the build directory...
)

target_link_libraries(simplenotes
        Qt::Core
        Qt::Gui
        Qt::Widgets
        qmarkdowntextedit
        ryml
)

