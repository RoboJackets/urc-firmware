# Compiling with PlatformIO

There are many ways to use PlatformIO. Probably the easiest way to use PlatformIO is to use the blue bar at the bottom of VS Code.

![menu_bar](img/menu_bar.png)

* House Icon: PIO Home. Useful for creating new projects, opening an existing project, looking at examples, and browsing for libraries.
  
* Checkmark: Compiles your project. Outputs files inside the ```.pio/build``` directory.
  
* Arrow: Uploads your compiled code to a microcontroller, if its plugged into your computer.
  
* Garbage Can: Similar to ```make clean```; clears out the build directory in ```.pio/build```. If you suspect that old compiled files could be messing with compilation, clicking this is probably a good idea.
  
* Beaker: Run unit tests. We don't have any tests, so this isn't useful.
  
* Electrical Plug: Serial monitor. If you have a microcontroller plugged into your computer, you should open the Serial monitor. If you have ```Serial.print``` statements in your code, the microcontroller will print to the Serial monitor.
  
* Box with Carrot: Opens a terminal. Simply opens a terminal window in VS Code in the project directory.
  
* Folder Icon: Displays the currently selected PlatformIO project. When you compile code, PlatformIO is compiling the code for the project shown next to this folder.
