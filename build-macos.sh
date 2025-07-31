#!/bin/bash
set -e

# Desktop Prompt Manager macOS Application Bundle Builder
# This script creates a macOS .app bundle and optionally a .dmg installer

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_ROOT/build"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
    exit 1
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

# Check if we're on macOS
if [[ "$OSTYPE" != "darwin"* ]]; then
    error "This script must be run on macOS"
fi

# Parse command line arguments
CLEAN=false
CREATE_DMG=false
SKIP_DEPS=false
SIGN_BUNDLE=false
DEVELOPER_ID=""

while [[ $# -gt 0 ]]; do
    case $1 in
        --clean)
            CLEAN=true
            shift
            ;;
        --dmg)
            CREATE_DMG=true
            shift
            ;;
        --skip-deps)
            SKIP_DEPS=true
            shift
            ;;
        --sign)
            SIGN_BUNDLE=true
            shift
            ;;
        --developer-id)
            DEVELOPER_ID="$2"
            SIGN_BUNDLE=true
            shift 2
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo "Build Desktop Prompt Manager application bundle for macOS"
            echo ""
            echo "Options:"
            echo "  --clean           Clean build directory before building"
            echo "  --dmg             Create DMG installer after building"
            echo "  --skip-deps       Skip dependency checking"
            echo "  --sign            Sign the application bundle"
            echo "  --developer-id ID Specify Developer ID for signing"
            echo "  -h, --help        Show this help message"
            exit 0
            ;;
        *)
            error "Unknown option: $1"
            ;;
    esac
done

log "Starting Desktop Prompt Manager macOS application build..."

# Clean build directory if requested
if [ "$CLEAN" = true ]; then
    log "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Extract version from CMakeLists.txt
VERSION=$(grep "project.*VERSION" "$PROJECT_ROOT/CMakeLists.txt" | sed -n 's/.*VERSION \([0-9.]*\).*/\1/p')
if [ -z "$VERSION" ]; then
    VERSION="1.0.0"
    warn "Could not extract version from CMakeLists.txt, using default: $VERSION"
fi

log "Building version: $VERSION"

# Check dependencies unless skipped
if [ "$SKIP_DEPS" = false ]; then
    log "Checking build dependencies..."
    
    # Check for Qt6
    if ! command -v qmake6 &> /dev/null && ! command -v qmake &> /dev/null; then
        error "Qt6 not found. Install with: brew install qt@6"
    fi
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        error "CMake not found. Install with: brew install cmake"
    fi
    
    # Check for macdeployqt
    QT_DIR=""
    if command -v qmake6 &> /dev/null; then
        QT_DIR=$(qmake6 -query QT_INSTALL_PREFIX)
    elif command -v qmake &> /dev/null; then
        QT_DIR=$(qmake -query QT_INSTALL_PREFIX)
    fi
    
    MACDEPLOYQT="$QT_DIR/bin/macdeployqt"
    if [ ! -f "$MACDEPLOYQT" ]; then
        error "macdeployqt not found at $MACDEPLOYQT"
    fi
    
    success "All dependencies found"
fi

# Configure and build with CMake
log "Configuring project with CMake..."

cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

log "Building project..."
make -j$(sysctl -n hw.ncpu)

# Check for the Qt-generated .app bundle
APP_NAME="PromptManagerDesktop.app"
APP_PATH="$BUILD_DIR/$APP_NAME"
CONTENTS_DIR="$APP_PATH/Contents"
MACOS_DIR="$CONTENTS_DIR/MacOS"
RESOURCES_DIR="$CONTENTS_DIR/Resources"

if [ ! -d "$APP_PATH" ] || [ ! -f "$MACOS_DIR/PromptManagerDesktop" ]; then
    error "Build failed - .app bundle not found at $APP_PATH"
fi

success "Build completed successfully"

log "Updating application bundle..."

# Ensure Resources directory exists
mkdir -p "$RESOURCES_DIR"

# Update Info.plist with additional properties (Qt already created one)
if [ -f "$CONTENTS_DIR/Info.plist" ]; then
    chmod +w "$CONTENTS_DIR/Info.plist"
fi
cat > "$CONTENTS_DIR/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>PromptManagerDesktop</string>
    <key>CFBundleIdentifier</key>
    <string>com.promptmanager.PromptManagerDesktop</string>
    <key>CFBundleName</key>
    <string>Prompt Manager Desktop</string>
    <key>CFBundleDisplayName</key>
    <string>Prompt Manager Desktop</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>????</string>
    <key>NSRequiresAquaSystemAppearance</key>
    <false/>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHumanReadableCopyright</key>
    <string>© 2025 Prompt Manager Team</string>
    <key>LSApplicationCategoryType</key>
    <string>public.app-category.productivity</string>
</dict>
</plist>
EOF

# Copy icon if it exists
ICON_FOUND=false
for icon_path in "resources/icons/appicon.icns" "resources/icons/icon.png" "resources/icon.png" "icon.icns" "icon.png"; do
    if [ -f "$PROJECT_ROOT/$icon_path" ]; then
        if [[ "$icon_path" == *.icns ]]; then
            cp "$PROJECT_ROOT/$icon_path" "$RESOURCES_DIR/PromptManagerDesktop.icns"
            /usr/libexec/PlistBuddy -c "Add :CFBundleIconFile string PromptManagerDesktop.icns" "$CONTENTS_DIR/Info.plist" 2>/dev/null || true
            log "Using ICNS icon for app bundle"
        elif [[ "$icon_path" == *.png ]] && command -v sips &> /dev/null; then
            log "Converting PNG icon to ICNS..."
            sips -s format icns "$PROJECT_ROOT/$icon_path" --out "$RESOURCES_DIR/PromptManagerDesktop.icns" &>/dev/null
            /usr/libexec/PlistBuddy -c "Add :CFBundleIconFile string PromptManagerDesktop.icns" "$CONTENTS_DIR/Info.plist" 2>/dev/null || true
        fi
        ICON_FOUND=true
        break
    fi
done

if [ "$ICON_FOUND" = false ]; then
    warn "No icon found, application will use default icon"
fi

# Deploy Qt frameworks
log "Deploying Qt frameworks..."

# Find macdeployqt if not already set
if [ -z "$MACDEPLOYQT" ]; then
    QT_DIR=""
    if command -v qmake6 &> /dev/null; then
        QT_DIR=$(qmake6 -query QT_INSTALL_PREFIX)
    elif command -v qmake &> /dev/null; then
        QT_DIR=$(qmake -query QT_INSTALL_PREFIX)
    fi
    
    MACDEPLOYQT="$QT_DIR/bin/macdeployqt"
    if [ ! -f "$MACDEPLOYQT" ]; then
        error "macdeployqt not found at $MACDEPLOYQT"
    fi
fi

QML_DIR="$PROJECT_ROOT/qml"
if [ -d "$QML_DIR" ]; then
    "$MACDEPLOYQT" "$APP_PATH" -qmldir="$QML_DIR" -verbose=2
else
    "$MACDEPLOYQT" "$APP_PATH" -verbose=2
fi

# Sign the bundle if requested
if [ "$SIGN_BUNDLE" = true ]; then
    log "Signing application bundle..."
    
    if [ -z "$DEVELOPER_ID" ]; then
        # Try to find a valid Developer ID
        DEVELOPER_ID=$(security find-identity -v -p codesigning | grep "Developer ID Application" | head -n 1 | sed 's/.*"\(.*\)".*/\1/')
    fi
    
    if [ -n "$DEVELOPER_ID" ]; then
        log "Signing with: $DEVELOPER_ID"
        codesign --force --options runtime --sign "$DEVELOPER_ID" "$APP_PATH"
        success "Application signed successfully"
    else
        warn "No Developer ID found, skipping signing"
    fi
fi

success "Application bundle created: $APP_PATH"

# Create DMG if requested
if [ "$CREATE_DMG" = true ]; then
    log "Creating DMG installer..."
    
    DMG_NAME="PromptManagerDesktop-$VERSION.dmg"
    DMG_PATH="$BUILD_DIR/$DMG_NAME"
    TEMP_DMG="$BUILD_DIR/temp.dmg"
    
    # Remove existing DMG
    rm -f "$DMG_PATH" "$TEMP_DMG"
    
    # Create temporary DMG
    hdiutil create -srcfolder "$APP_PATH" -volname "Prompt Manager Desktop" -fs HFS+ -fsargs "-c c=64,a=16,e=16" -format UDRW "$TEMP_DMG"
    
    # Mount the DMG
    MOUNT_DIR=$(hdiutil attach -readwrite -noverify -noautoopen "$TEMP_DMG" | egrep '^/dev/' | sed 1q | awk '{print $3}')
    
    # Create Applications symlink
    ln -sf /Applications "$MOUNT_DIR/Applications"
    
    # Set background and icon positions (if Finder is available)
    if command -v osascript &> /dev/null; then
        osascript << EOF
tell application "Finder"
    tell disk "Prompt Manager Desktop"
        open
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set the bounds of container window to {400, 100, 900, 450}
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to 128
        set position of item "PromptManagerDesktop.app" of container window to {150, 200}
        set position of item "Applications" of container window to {350, 200}
        close
        open
        update without registering applications
        delay 2
    end tell
end tell
EOF
    fi
    
    # Unmount the DMG
    hdiutil detach "$MOUNT_DIR"
    
    # Convert to compressed DMG
    hdiutil convert "$TEMP_DMG" -format UDZO -imagekey zlib-level=9 -o "$DMG_PATH"
    rm -f "$TEMP_DMG"
    
    success "DMG created: $DMG_PATH"
fi

# Show bundle info
log "Application bundle information:"
echo "  Bundle: $APP_PATH"
echo "  Version: $VERSION"
echo "  Size: $(du -sh "$APP_PATH" | cut -f1)"

if [ "$CREATE_DMG" = true ]; then
    echo "  DMG: $DMG_PATH"
    echo "  DMG Size: $(du -sh "$DMG_PATH" | cut -f1)"
fi

success "Prompt Manager Desktop macOS build completed!"