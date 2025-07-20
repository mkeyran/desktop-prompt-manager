# Prompt Manager Desktop

A desktop application for managing AI prompts with dynamic placeholders, built with Qt 6, C++, and QML. This is a desktop equivalent of the Android Prompt Manager application.

## Features

- **Prompt Management**: Create, edit, delete, and organize prompts
- **Dynamic Placeholders**: Use `{{placeholder}}` syntax for dynamic content
- **Folder Organization**: Organize prompts into folders (optional)
- **Search Functionality**: Full-text search across prompt titles and content
- **Interactive Placeholder Filling**: Step-by-step placeholder completion with live preview
- **Modern UI**: Built with QML and Qt Quick Controls for a responsive interface

## Architecture

### MVVM Pattern
- **Models**: `Prompt`, `Folder`, `PromptWithFolder` (C++ QObject-based models)
- **Views**: QML screens and components
- **ViewModels**: `PromptListViewModel`, `PromptEditViewModel`, `PlaceholderViewModel`

### Core Components
- **Database Layer**: SQLite with Qt SQL (Database, PromptDao, FolderDao)
- **Repository Pattern**: `PromptRepository` for data access abstraction
- **Utility Classes**: `PlaceholderUtils` for `{{placeholder}}` processing

### UI Structure
- **Screens**: PromptListScreen, PromptEditScreen, PlaceholderFillingScreen
- **Components**: PromptCard, SearchBar, FolderChip, CustomButton

## Requirements

- Qt 6.2 or later
- CMake 3.21 or later
- C++17 compatible compiler
- SQLite (included with Qt)

## Building

### Prerequisites

Install Qt 6 development packages:

**Ubuntu/Debian:**
```bash
sudo apt install qt6-base-dev qt6-declarative-dev cmake build-essential
```

**macOS (with Homebrew):**
```bash
brew install qt@6 cmake
```

**Windows:**
- Install Qt 6 from https://www.qt.io/download
- Install CMake and Visual Studio or MinGW

### Build Steps

1. Clone or download the project
2. Run the build script:
   ```bash
   ./build.sh
   ```

Or build manually:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Running

```bash
cd build
./PromptManagerDesktop
```

## Usage

### Creating Prompts

1. Click "New Prompt" on the main screen
2. Enter a title and content
3. Optionally assign to a folder
4. Use `{{placeholder}}` syntax for dynamic content
5. Click "Save"

### Using Placeholders

Placeholders use the syntax `{{placeholder_name}}`. Examples:
- `Hello {{name}}, welcome to {{company}}!`
- `Generate a {{type}} about {{topic}} in {{style}} style`

### Filling Placeholders

1. From the prompt list, click a prompt with placeholders
2. Click "Fill Placeholders" 
3. Enter values step-by-step
4. Preview the result in real-time
5. Copy the final result

### Managing Folders

1. Create folders from the "New Prompt" screen
2. Filter prompts by folder using the folder chips
3. Delete folders (prompts are preserved)

### Searching

- Use the search bar to find prompts by title or content
- Combine search with folder filtering
- Search results update in real-time (300ms debounce)

## Database Schema

### Prompts Table
- `id` (Primary Key)
- `title` (Text)
- `content` (Text) 
- `folder_id` (Foreign Key, nullable)
- `created_at` (Timestamp)
- `updated_at` (Timestamp)

### Folders Table
- `id` (Primary Key)
- `name` (Text, unique)
- `created_at` (Timestamp)
- `updated_at` (Timestamp)

### Relationships
- One-to-Many: Folder → Prompts (optional)
- Foreign key constraint with SET NULL on folder deletion

## Project Structure

```
src/
├── main.cpp                 # Application entry point
├── models/                  # Data models
│   ├── prompt.h/cpp
│   ├── folder.h/cpp
│   └── promptwithfolder.h/cpp
├── database/               # Database layer
│   ├── database.h/cpp
│   ├── promptdao.h/cpp
│   └── folderdao.h/cpp
├── repository/             # Repository pattern
│   └── promptrepository.h/cpp
├── viewmodels/             # MVVM ViewModels
│   ├── promptlistviewmodel.h/cpp
│   ├── prompteditviewmodel.h/cpp
│   └── placeholderviewmodel.h/cpp
└── utils/                  # Utility classes
    ├── placeholderutils.h/cpp
    └── searchfilter.h/cpp

qml/
├── main.qml               # Main application window
├── screens/               # Application screens
│   ├── PromptListScreen.qml
│   ├── PromptEditScreen.qml
│   └── PlaceholderFillingScreen.qml
└── components/            # Reusable UI components
    ├── PromptCard.qml
    ├── SearchBar.qml
    ├── FolderChip.qml
    ├── CustomButton.qml
    └── ConfirmDialog.qml
```

## Key Design Decisions

### Qt/C++ vs Android Equivalents
- **Room Database** → **Qt SQL with SQLite**
- **Android ViewModels** → **Qt QObject-based ViewModels with Q_PROPERTY**
- **StateFlow** → **Qt Signals/Slots with QProperty**
- **Jetpack Compose** → **QML Components**
- **Android Navigation** → **QML StackView navigation**

### State Management
- ViewModels use Qt properties with change notifications
- Repository emits signals for data changes
- UI updates reactively through property bindings

### Data Persistence
- SQLite database in application data directory
- Database schema matches Android version
- Foreign key constraints ensure data integrity

## Contributing

This project demonstrates Qt/C++/QML equivalent architecture to the Android Prompt Manager application. Key areas for enhancement:

1. **Advanced Search**: Full-text search with ranking
2. **Export/Import**: JSON/CSV export functionality  
3. **Themes**: Dark/light theme support
4. **Cloud Sync**: Optional cloud synchronization
5. **Plugin System**: Custom placeholder processors
6. **Performance**: Virtual list views for large datasets

## License

This project is created as a demonstration of Qt/C++/QML architecture equivalent to Android development patterns.