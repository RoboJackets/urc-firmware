# Interfacing SHT3X Temperature & Humidity Sensors

## Code Implementation

Begin by downloading the [starter code from DFRobot's website](https://wiki.dfrobot.com/Digital_Temperature_and_Humidity_sensor__With_Stainless_Steel_Probe___SKU_SEN0148_).

Based on your device model, you may need to be mindful of how you create instances of the `DFRobot_SHT3x` class. For SEN0148, the stainless-steel model, you will need to provide the correct arguments to the class constructor. The I2C address is 0x44 and the internal reset pin of the driver integrated circuit is 4.

```cpp
#include <DFRobot_SHT3x.h>

DFRobot_SHT3x sht3x(&Wire, 0x44, 4);
```

>**Note:** `DFRobot_SHT3x.h` imports `Wire.h`, allowing `Wire` to be resolved.

## Testing

[DFRobot's library GitHub Repository](https://github.com/DFRobot/DFRobot_SHT3x/tree/master) contains multiple Arduino sketches in the `examples/` directory. Clone the repository locally and open `periodicDataReading.ino`. **Change the initialization of the `sht3x` object, as shown above.**

Upload the sketch to your board. Verify that reasonable temperature and humidity values are displayed in the Serial Console.

```
------------------Read data in cycle measurement mode-----------------------
chip serial number: 269652062
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
Ambient temperature(°C/F):18.98 C/66.16 F Relative humidity(%RH):31.07 %RH
```
