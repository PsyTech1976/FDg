cmake_minimum_required(VERSION 3.16)

# Definizione del progetto FDg
project(fdg VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Abilitazione automatica di MOC, UIC e RCC per Qt
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Ricerca dei moduli Qt6 necessari
find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets DBus)

# Inclusione delle cartelle header
include_directories(src)

# Sorgenti dell'applicazione
set(SOURCES
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/mainwindow.ui
    src/fdrunner.h
    src/fdrunner.cpp
    src/filemanagerhelper.h
    src/filemanagerhelper.cpp
    src/guidedialog.h
    src/guidedialog.cpp
    src/desktopintegrator.h
    src/desktopintegrator.cpp
    src/dependencyinstaller.h
    src/dependencyinstaller.cpp
    resources/resources.qrc
)

# Creazione dell'eseguibile principale FDg
add_executable(fdg ${SOURCES})

# Collegamento con i moduli Qt6
target_link_libraries(fdg PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::DBus
)

# Regole di installazione per generazione pacchetti e AppImage
include(GNUInstallDirs)
install(TARGETS fdg DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES packaging/fdg.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES resources/icons/appicon.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)
install(FILES resources/icons/appicon_256.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/256x256/apps RENAME appicon.png)
install(FILES resources/icons/appicon.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps)
