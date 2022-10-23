## How to use Static Code Analysis in PlatformIO

In the GUI, go to PlatformIO Home (the house icon). On the home page, click on the `Inspect` tab.
Select the urc-firmware PlatformIO project, and leave both `Inspect Memory` and `Check Code` options on. Then, click `Inspect`.
Wait a minute or two for the analysis to complete.

You can also do this from the command line if you have [PlatformIO CLI](../installation/installation.md) installed. Run the following in the same directory as the `platformio.ini` file:

```bash
pio check
```

### [Return to Documents](../README.md)