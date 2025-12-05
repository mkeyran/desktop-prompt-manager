cmake_minimum_required(VERSION 3.21)

project(PromptManagerDesktop VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Quick Sql)

qt_standard_project_setup()

# Set Qt policies to suppress warnings
if(POLICY CMP0154)
    cmake_policy(SET CMP0154 NEW)
endif()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

# Source files
set(SOURCES
    src/main.cpp
    src/models/prompt.cpp
    src/models/folder.cpp
    src/models/promptwithfolder.cpp
    src/database/database.cpp
    src/database/promptdao.cpp
    src/database/folderdao.cpp
    src/database/folderdao.cpp
    src/repository/promptrepository.cpp
    src/repository/sqlpromptrepository.cpp
    src/repository/markdownpromptrepository.cpp
    src/viewmodels/promptlistviewmodel.cpp
    src/viewmodels/prompteditviewmodel.cpp
    src/viewmodels/placeholderviewmodel.cpp
    src/utils/placeholderutils.cpp
    src/utils/searchfilter.cpp
    src/utils/clipboardutils.cpp
)

# Header files
set(HEADERS
    src/models/prompt.h
    src/models/folder.h
    src/models/promptwithfolder.h
    src/database/database.h
    src/database/promptdao.h
    src/database/folderdao.h
    src/database/folderdao.h
    src/repository/promptrepository.h
    src/repository/sqlpromptrepository.h
    src/repository/markdownpromptrepository.h
    src/viewmodels/promptlistviewmodel.h
    src/viewmodels/prompteditviewmodel.h
    src/viewmodels/placeholderviewmodel.h
    src/utils/placeholderutils.h
    src/utils/searchfilter.h
    src/utils/clipboardutils.h
    src/utils/settingsmanager.h
)

# QML files
set(QML_FILES
    qml/main.qml
    qml/screens/PromptListScreen.qml
    qml/screens/PromptEditScreen.qml
    qml/screens/PlaceholderFillingScreen.qml
    qml/components/PromptCard.qml
    qml/components/SearchBar.qml
    qml/components/FolderChip.qml
    qml/components/CustomButton.qml
    qml/components/ConfirmDialog.qml
    qml/components/SettingsDialog.qml
)

qt_add_executable(PromptManagerDesktop ${SOURCES} ${HEADERS})

qt_add_qml_module(PromptManagerDesktop
    URI PromptManager
    VERSION 1.0
    QML_FILES ${QML_FILES}
)

target_link_libraries(PromptManagerDesktop PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Quick
    Qt6::Sql
)

# Set application properties
set_target_properties(PromptManagerDesktop PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.promptmanager
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Create assets directory if needed
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/assets)