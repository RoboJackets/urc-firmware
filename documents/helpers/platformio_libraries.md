# Libraries in PlatformIO

Usually, libraries are a huge pain when working with firmware. PlatformIO makes using libraries much easier.

The ```platformio.ini``` file is where you declare the libraries you are using. In particular, the ```lib_deps``` line lists all of the libraries you are using. Here is an example:
```
lib_deps = 
    fastled/FastLED 
    https://github.com/adafruit/Adafruit_NeoPixel.git
    https://github.com/me-no-dev/AsyncTCP/archive/master.zip
    /path/to/custom/library
```
The above example displays the 4 different ways you can include libraries.

* fastled/FastLED: This library is in the PlatformIO registry. These libraries work really well with PlatformIO! 
    * You can search the PlatformIO registry [here](https://registry.platformio.org/), or you can do it inside VS code by navigating to ```PIO Home > Libraries``` and search the registry from there. 
    * [Here](https://registry.platformio.org/libraries/fastled/FastLED) is a link to the FastLED registry page. 
        * The ```Installation``` page gives you the string you need to put in the ```lib_deps``` section to include the library in you project (i.e. ``` lib_deps = fastled/FastLED ```). 
        * The ```Examples``` page gives you example programs using the library. You should check this out to figure out how to include and use the library. For example, check out [this](https://registry.platformio.org/libraries/fastled/FastLED/examples/Blink/Blink.ino) blink program for the FastLED library. This example tells you that you need to put  ```#include <FastLED.h>``` to include the library.

* https://github.com/adafruit/Adafruit_NeoPixel.git: This is simply a link to a Github project; PlatformIO will automatically download it! You should always check the PlatformIO registry to see if the Github project is already in the registry. If it's not, don't worry! You may have to look a bit harder to figure out how to include and use the libraries, but the library inclusion process should be just as easy.
  
* https://github.com/me-no-dev/AsyncTCP/archive/master.zip: This is a link to a .zip library file. It can be local or remote.
  
* /path/to/custom/library: This specifies a path to a local library.

## Important Notes about including libraries

Firstly, be sure to include the library in the ```lib_deps``` line in the ```platformio.ini``` file AND in the ```#include``` statement(s) in the source or header files before compiling. Otherwise, you may get errors.

Secondly, some library files have dependecies that it expects you to include besides the library itself. This problem may happen if you try to include one library, but the compiler says it cannot find a different library that the library you included depends on. The easiest way to fix this is to find installation instructions for the library or an example. You will need multiple ```#include``` statements, and you may need to install another library.

### [Return to Documents](../README.md)