cmake_minimum_required(VERSION 3.20)
set(PROJECT_NAME "Windicator")
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)

add_compile_definitions(_UNICODE UNICODE)

# Uncomment the following line to add timeout to wait so that the
# registry handle can be released.  Not really needed and causes
# a very tiny bit of extra CPU to keep checking for exit condition.
#
#add_compile_definitions(_TIDY_TIMEOUT)

add_link_options(/SUBSYSTEM:WINDOWS /DYNAMICBASE "Version.lib" /MACHINE:X64)

add_executable(
        ${PROJECT_NAME}
        resources/resource.h
        resources/WinApp.rc
        include/messages.h
        include/aboutDialog.h
        src/aboutDialog.cpp
        include/MainWindow.h
        src/MainWindow.cpp
        include/notificationIcon.h
        src/notificationIcon.cpp
        include/notificationIcon.h
        src/notificationIcon.cpp
        include/desktopWatcher.h
        src/desktopWatcher.cpp
        src/winMain.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
