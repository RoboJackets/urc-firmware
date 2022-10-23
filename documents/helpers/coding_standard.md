# C Coding Standard

We will use code formatting standards outlined [here](https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html#brace).
Before contributing, be sure to read these over.

## Variable Naming Convention

```
Struct:         my_struct
Struct Member:  my_struct_member

Enum:           MyEnumName
Enum Member:    MY_ENUM_MEMBER

Macros:         MY_MACRO

Local Vars:     my_local_variable
Global Vars:    g_my_global_variable
Global Const:   MY_GLOBAL_CONST
```

## Clang Format

Clang format is a nice tool for doing simple code formatting. To run `clang format`, go to the main project directory and run

```
make format
```

### [Return to Documents](../README.md)