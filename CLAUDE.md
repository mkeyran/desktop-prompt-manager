# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

**Build the application:**
```bash
./build.sh
```

**Manual build:**
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

**Run the application:**
```bash
cd build && ./PromptManagerDesktop
```

**Note:** This project uses CMake with Qt6. No test suite is currently configured.

## Architecture Overview

This is a Qt6/C++/QML desktop application using MVVM architecture pattern. The application manages AI prompts with dynamic placeholder functionality.

### Key Architectural Components

**MVVM Pattern:**
- **Models**: C++ QObject-based classes with Q_PROPERTY for QML binding (`Prompt`, `Folder`, `PromptWithFolder`)
- **ViewModels**: Bridge between C++ business logic and QML UI (`PromptListViewModel`, `PromptEditViewModel`, `PlaceholderViewModel`)
- **Views**: QML screens with StackView navigation (`PromptListScreen`, `PromptEditScreen`, `PlaceholderFillingScreen`)

**Data Flow:**
```
QML UI ↔ ViewModel ↔ Repository ↔ DAO ↔ SQLite Database
```

**Navigation:** StackView-based with three main screens. Navigation signals flow from child components up to main.qml which manages the stack.

### Database Architecture

**SQLite with Qt SQL:**
- **Database**: Singleton connection manager
- **DAOs**: `PromptDao`, `FolderDao` handle CRUD operations
- **Repository**: `PromptRepository` provides unified data access with signal-based notifications
- **Schema**: Two tables (prompts, folders) with foreign key relationships

### Signal Architecture

The application uses Qt's signal/slot system extensively:
- Repository emits signals for data changes (`promptAdded`, `promptUpdated`, etc.)
- ViewModels emit property change signals for UI updates
- All data modifications flow through repository to ensure consistent notifications

## Important Implementation Details

### Placeholder System
- Uses `{{placeholder}}` syntax processed by `PlaceholderUtils`
- `PlaceholderViewModel` manages step-by-step filling with live preview
- Recent refactor replaced step navigation with repeater showing all placeholders simultaneously

### Search Implementation
- `PromptListViewModel` has debounced search (300ms timer) via `searchTextChanged` signal
- Search is full-text across title and content fields
- Combines with folder filtering for refined results

### QML Resource System
- QML files are embedded in executable via Qt resource system (`qml.qrc`)
- Changes to QML files require rebuild (no hot-reload)
- QML module registered as "PromptManager" version 1.0

### ViewModels
- `PromptListViewModel`: Inherits `QAbstractListModel` for efficient list display
- `PlaceholderViewModel`: Manages current index, values map, and processed content
- All ViewModels use Q_PROPERTY with NOTIFY signals for reactive UI updates

## Code Organization

**Core Directories:**
- `src/`: C++ source organized by layer (models, database, repository, viewmodels, utils)
- `qml/`: QML UI files (screens, components)
- `build/`: CMake build artifacts (excluded from git)

**Key Files:**
- `main.cpp`: Registers QML types and sets up ViewModels
- `main.qml`: Application window with StackView navigation
- `CMakeLists.txt`: Qt6 modern CMake configuration

## Development Notes

**Qt6 Specifics:**
- Uses `qt_standard_project_setup()` for modern CMake integration
- Requires Qt6 Core, Gui, Quick, Sql components
- C++17 standard

**Database Location:**
- SQLite database auto-initializes in platform-specific AppDataLocation
- Database schema matches Android version for cross-platform consistency

**UI Patterns:**
- Property bindings for automatic UI synchronization
- Custom components follow consistent styling patterns
- Signal-based navigation between screens