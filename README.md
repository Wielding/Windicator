# Windicator

**Windows Defender is reporting that this executable contains Trojan:Win32/Sabsik.FL.A!ml and if not that  
Trojan:Win32/Wacatac.B!ml or even Win32/Wacapew.C!ml. I have tried submitting it as a false positive, but I don't know
if that will ever go through. If want to use it you will have to build it yourself since I have removed the releases
until I can sort this out**

This is a simple Windows 11 application that shows an indicator in the system tray displaying the current virtual
desktop number. It does not work on Windows 10 due to Microsoft changing the registry keys.

It might also require the latest Windows 11 updates since I found it does not work on an older Windows 11 that I have in
a VM. It works on my current desktop and laptop, so I am happy and don't feel like doing that much investigation. I
don't have the time to set up development environments on other Windows versions. If it does not work for you, I am
sorry. Feel free to fork the project and fix it for older versions if you wish. The beauty of Open Source. I might not
want to incorporate legacy code in this repository so forking is probably the best option if you want to modify it.

It maxes out at 10 virtual desktops since anything more than 1 digit is difficult to show nicely in the tray icon.
Desktop 10 is shown as '0' and Anything past that displays an '*'. If you use more than 10 virtual desktops I think you
might have a workflow problem to address.

The application monitors the registry for changes to your virtual desktops and then updates the notification icon with
the desktop number. It uses events to wait for registry changes so there is no activity in the background unless you
change desktops. It uses very little memory and virtually no CPU.

There is no installer for this application since it is a single Windows executable. Just put it wherever you want it and
run it. There is no configuration or command line options. You can even just drop it into your Startup folder so that it
automatically runs when you log in.

If you are just downloading the release executable you will need the latest Microsoft Visual C++ Redistributable if you
don't have it already. You can check if you already have it by just running the application. If you don't have the
runtime a dialog will pop up telling you that it is missing. If that happens the runtime can be downloaded directly from
[here](https://aka.ms/vs/17/release/vc_redist.x64.exe) or navigate to
https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0
and select the x64 download.

There will be more docs coming.... maybe... probably not. There is not much more to say about it.

## Roadmap (not likely)

1. Add icons specific for dark or light Windows mode
2. Add a floating OSD or maybe fading in/out effect for a more obvious desktop switching visual since Microsoft decided
   to do away with the virtual desktop switching animations in Windows 11. I will hold off on this for a while since
   Microsoft might bring back that functionality once they figure out the performance issues it was causing.
3. Make it compatible with Windows 10.

## Building

If you want to build it you need to make sure you have Visual Studio 2022 with the CMake tools installed. Otherwise, you
can download cmake and the VC build tools manually to build it.

I prefer to use CMake to Visual Studio solution files for several reasons.

1. So that I can use other IDEs (e.g. Visual Studio Code or CLion) to write my Windows applications. Visual Studio is a
   big install and a bit of a resource hog, so I use VSCode on my laptop while I am on the go.
2. CMakeLists.txt files are much easier to read and modify compared to Visual Studio Solution Files.
3. They are multi-platform, so I can use my CMake knowledge for my Linux projects.

