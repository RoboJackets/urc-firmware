# Compiling Nanopb with Arduino

Before following these instructions, make sure that you have installed the Arduino IDE and Teensyduino. 

## What is Nanopb?

Protobuf (short for "protocol buffers") is a data formatting scheme used to transmit data, usually across a network. We are using an implementation of Protobuf called Nanopb for URC. In particular, we use nanopb to pass data from the computer running ROS to the microcontrollers connected to the same network.

To get a quick introduction to Nanopb, [look here](https://jpa.kapsi.fi/nanopb/docs/index.html).

Basically, we have a ```.proto``` file which specifies what our message will look like. Then, we feed the ```.proto``` file into a python script called [```nanopb_generator.py```](https://github.com/nanopb/nanopb/blob/master/generator/nanopb_generator.py), which produces a ```.pb.c``` file and a ```.pb.h``` file. Finally, when code that uses the nanopb messages defined in the ```.proto``` file, the compiler uses the ```.pb.c``` and the ```.pb.h``` files.

## Setup: Adding the Nanopb Arduino Library

To use crucial Nanopb library functions, we need to add Nanopb as a library to Arduino. Unfortunately, Nanopb isn't avaliable as a typical Arduino library. Therefore, we must add it ourselves.

## 1. Make sure Nanopb is present as a submodule
``` 
git submodule update --init
```

After updating submodules, you should should see the ```nanopb``` repository inside the ```external``` folder.

## 2. Locate your Arduino Library Folder

Next, locate your Arduino libraries folder. On linux, the folder usually is ```~/Arduino/libraries```.

## 3. Create a Nanopb library

Create a folder called ```Nanopb``` in the Arduino libraries folder.
```
mkdir ~/Arduino/libraries/Nanopb
```
Next copy the following files into the Arduino libraries folder:
* ```pb.h ```
* ```pb_common.c``` and ```pb_common.h```
* ```pb_encode.c``` and ```pb_encode.h```
* ```pb_decode.c``` and ```pb_decode.h```

These files are located in ```urc-firmware/external/nanopb```. To do this on Linux, enter:
```
cp external/nanopb/pb* ~/Arduino/libraries/Nanopb/
```

Finally, change all ```.c``` files to ```.cpp```. This helps ```Nanobp``` show up in the Arduino IDE.

## 4. Include the Nanopb Header Files

To include the Nanopb files in your Arduino code, type:
```
#include <pb_decode.h>
#include <pb.h>
#include <pb_encode.h>
#include <pb_common.h>
```
You should also see the ```Nanopb``` library in the ```Include Library``` folder.

## 5. Generate the Nanopb files

Now, take a ```.proto``` file. We need to turn the ```.proto``` file in a C source and header file. To do this, you need to run the ```nanopb_generator.py``` script.
```
python3 external/nanopb/generator/nanopb_generator.py src/nanopb_messages/urc.proto
```

To use the generated files, include the header file. For instance, if the ```.proto``` file is ```urc.proto```, then include:
```
#include "urc.pb.h"
```

### Hopefully it compiles!

That's it! You should be able to encode, decode, and handle Nanopb messages!