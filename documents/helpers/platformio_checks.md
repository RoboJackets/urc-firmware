# MISRA C

MISRA (Motor Industry Software Reliability Association) C is a set of software development guidelines specifically for embedded systems. We are using the MISRA C Cppcheck addon for static code analysis.

The website below lists out all of the MISRA C 2012 guidelines with examples:

https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html


## How to use Static Code Analysis in PlatformIO

In the GUI, go to PlatformIO Home (the house icon). On the home page, click on the `Inspect` tab.
Select the urc-firmware PlatformIO project, and leave both `Inspect Memory` and `Check Code` options on. Then, click `Inspect`.
Wait a minute or two for the analysis to complete.

The section we care about is the `Defects` section. You can sort the defects by clicking on the various column
headers. The `Location` column specifies where the defect is in the source code. If you hover over the 
message, it will show the cppcheck defect ID. For example, it might say `cppcheck defect ID: misra-c2012-8.9`.
To find more information about this defect go to the [MISRA C 2012 guidelines](https://docs.zephyrproject.org/latest/contribute/coding_guidelines/index.html) and search for the rule number. If the cppcheck ID was `cppcheck defect ID: misra-c2012-8.9`, look at Rule 8.9. You can click on the hyperlink in the `Example` column to see examples of compliant and non-compliant code. 

You can also do this from the command line if you have [PlatformIO CLI](../installation/installation.md) installed. Run the following in the same directory as the `platformio.ini` file:

```bash
pio check
```

## How to Approach MISRA C Rules and Guidelines

MISRA C rules are either `Required` or `Advisory`. The `Required` rules should be followed quite strictly, but the `Advisory` rules can be broken if you have a good reason.

You don't need to get rid of all of the defects; the MISRA C Rules are there to help promote better code. However, 
the guidelines are tailored for embedded applications and are there for a good reason. If you are unsure of why
a certain MISRA C rule exists, do some research! You will learn a lot about C if you understand the rationale 
behind the rules. 


### [Return to Documents](../README.md)