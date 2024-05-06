# How to add a new expansion to Opta BluePrint

==============================================

## Introduction

This little guide is intended for people that want to add the support for a new
expansion to Opta BluePrint.

To add new expansion to the BluePrint library is necessary to provide:

- a new FW (this is the 'sketch' running on the expansion hardware that receives
  command from Opta Controller and send back information)
- a new <Something>Expansion class which is derived from the Expansion class
  contained in this Library (in the following this be referred as the 'expansion
  class', this is intended to be all the necessary files to have a fully usable
  expansion on the Opta Controller)

The FW and expansion class must be developed in a different folder/repository
and no changes or PR are required.

It is suggested that FW source files and the expansion class are put in the
same folder for simplicity reasons (this allows to share information between
the FW and expansion class in a easy way).

## How to make a new FW

The FW will be composed by the following file

- An Opta<Something>.h file
- An Opta<Something>.cpp file
- An Opta<Something>Cfg.h file (optional ?)
- A <Something>.ino sketch that will be the entry point to use implementation
  in the Opta<Something> files

Please note that the filename convention indicated above is just to keep
consistency with the file naming used in BluePrint library, but it is optional.

We recommend the sketch <Something>.ino to be placed in a different folder
(something like 'firmware'): for example the Digital expansion FW resides in
folder ./firmware/Digital/Digital.ino but it uses the OptaDigital .h and .cpp in
the ./src folder

So a good starting point to develop an expansion FW is the following folder
structure (suppose your expansion is called "NewExpansion")

```
/rood_folder_of_new_expansion_library
|-firmware/NewExpansion/NewExpansion.ino
|-src/
      |-OptaNewExpansion.h
      |-OptaNewExpansion.cpp
      |-OptaNewExpansionCfg.h (optional)
```

It is required that every FW version will be identified with a version number in
this form: MAJOR.MINOR.RELEASE
MAJOR, MINOR and release are bytes (uint8_t);

**_NOTE: in the following we suppose that the new expansion "name" is
"NewExpansion"_**

### OptaNewExpansion class

The files OptaNewExpansion file .h and .cpp must contain the implementation
of the class that handles the Expansion FW.

This class must inherit from Module class.
Module class declaration is contained in the OptaBlueModule.h header file so
that this file must be included in OptaNewExpansion.h

The class will be something like

```
class OptaNewExpansion : public Module {

};
```

The Module class defines some pure virtual functions so that every expansion
must implement these functions:

- `virtual uint8_t getMajorFw();` must return the Major FW version

- `virtual uint8_t getMinorFw();` must return the Minor FW version

- `virtual uint8_t getReleaseFw();` must return the Release FW version

- `virtual std::vector<uint8_t> getProduct();` must return a vector of bytes
  containing a description of the new expansion. This description will be used to
  identify in a univocal way the **_type_** of expansion. It must never happen
  that 2 different kind of expansions have the same description. The number of
  byte used is 32 (if the vector has more than 32 bytes, then only the first 32
  are actually used). Product description must be chosen carefully in order to
  avoid the fact that different producers use the same description, so a
  combination of the producer plus a unique identifier for the product should be
  enough. For example Opta Digital uses "Arduino Opta Digital" as product
  description (and only uses 21 byte).

- `virtual void goInBootloaderMode()` this function must implement a procedure to
  put the expansion in Bootloader mode in order to allow the upgrade of the FW. In
  case the Expansion does not provided the possibility to upgrade the FW just
  provide a dummy empty implementation.

- `virtual void readFromFlash(uint16_t add, uint8_t *buffer, uint8_t dim);` this
  function must implement the reading from flash (or anything similar) expansion
  device at address `add` of `dim` bytes, the bytes must be put into the `buffer`
  and the buffer must be at least `dim` long.
  Pay attention: read from flash function is in any case "driven" by an I2C
  transaction from Opta Controller so the maximum `dim` is 32 bytes (i.e. FLASH or
  EEPROM or any form of implementation is read 32 bytes at each transaction).
  Also the address is only 2 bytes wide because the I2C protocol reserve 2 bytes
  for the address.
  This is the default implementation you get for free by simply implementing
  this function (this means that at the controller level all is already
  implemented). Those limitations can be overcome by defining and handling a
  different I2C message.
  In case your expansion does not have this function just provide an empty,
  dummy implementation.

- `virtual void writeInFlash(uint16_t add, uint8_t *buffer, uint8_t dim);`this
  function must implement the writing into flash (or anything similar) expansion
  device at address `add` of `dim` bytes, the bytes copied from the
  `buffer` and so the buffer must be at least `dim` long.
  Pay attention: write into flash function is in any case "driven" by an I2C
  transaction from Opta Controller so the maximum writable `dim` is 32 bytes (i.e.
  FLASH or EEPROM or any form of implementation is written 32 bytes at each
  transaction).
  Also the address is only 2 bytes wide because the I2C protocol reserve 2 bytes
  for the address.
  This is the default implementation you get for free by simply implementing
  this function (this means that at the controller level all is already
  implemented). Those limitation can be overcome by defining
  and handling a different I2C message.
  In case your expansion does not have the possibility to write in flash just
  provide an empty, dummy implementation.

- `virtual void initStatusLED();` this function must provide a way to correctly
  initialize the HW for the LED status (on Arduino Opta expansions is present a
  RGB LED status which is used to signal the status of the expansion regarding
  the Address assignement: RED -> ready to get the I2C address, BLUE -> no
  address and waiting for other expansion to get their own address, GREEN -> I2C
  address acquired).
  In case Status led is not present just provide an empty, dummy implementation.

- `virtual void setStatusLedReadyForAddress();` this function must set the status
  LED in the state that indicates that the expansion has not I2C address but can
  acquire a new one.
  In case Status led is not present just provide an empty, dummy implementation.

- `virtual void setStatusLedWaitingForAddress();` this function must set the
  status LED in a state that indicates that the expansion has not address, but is
  not ready to acquire new one (typically is waiting for other expansions to get
  their address).
  In case Status led is not present just provide an empty, dummy implementation.

- `virtual void setStatusLedHasAddress();` this function must set the status LED
  in a state that indicates that the expansion has acquired a valid I2C address.
  In case Status led is not present just provide an empty, dummy implementation.

The OptaNewExpansion class constructor must call the base Module constructor
`Module(TwoWire *tw, int _detect_in, int _detect_out);` to inform the Module
class of what is the correct TwoWire object to be used (with the right pin SDA
and SCL) and which are the pin DETECT IN and DETECT OUT using during the Assign
Address process. (Note: this can be avoided, so that the base default Module
constructor is automatically called in case the expansion uses a core/variant
that define a Wire object on the right pins and the pin_arduino.h of the
core/variant contains two `#define` statements that correctly define DETECT_IN
and DETECT_OUT as the correct integer microcontroller PIN).

## OptaNewExpansionCfg.h

This is an optional configuration file that contains all the configuration
`#define` used to configure the FW.
If the file is present it must be included only in OptaNewExpansion.h file: this
means that this file will contain information relevant only to the FW level and
won't be "export" any information useful at the _application_ level (i.e. on the
Opta Controller side).

## Sharing information between FW and the controller

It might be good to have a place to put configurations or type definition that
are used both by FW and the Opta Controller application.
We suggest to call this file NewExpansionCommonCfg.h (always remember to use the
proper name of your expansion instead of 'NewExpansion' used here for simplicity).

This file will be included both in the OptaNewExpansion.h file (and this will
inform the FW) and in the NewExpansionExpansion.h (that name is ugly but it
simply means that the expansion class to be provided to the application /
controller level is <Something>Expansion and since we decided to use
'NewExpansion' as the name of our made up new expansion the name of the file
will NewExpansionExpansion.h)

So, at this point we can further populate our New Expansion library with the
files to be used at the controller/application level.

```
/rood_folder_of_new_expansion_library
|-firmware/NewExpansion/NewExpansion.ino
|-src/
      |-OptaNewExpansion.h
      |-OptaNewExpansion.cpp
      |-OptaNewExpansionCfg.h (optional)
      |-NewExpansionCommonCfg.h (common share configuration between FW and application)
      |-NewExpansionExpansion.h (header file for the expansion class at application level)
      |-NewExpansionExpansion.cpp (implementation file for the expansion at the application level)

```

Just for completeness the following expresses the "allocation" of the files to
FW or application.

```
/rood_folder_of_new_expansion_library
|-firmware/NewExpansion/NewExpansion.ino
|-src/
|-OptaNewExpansion.h     (FW header class)
|-OptaNewExpansion.cpp   (FW implementation class)
|-OptaNewExpansionCfg.h  (FW configuration, optional)
|-NewExpansionCommonCfg.h (FW / APP shared configuration and types)
|-NewExpansionExpansion.h (APP header class)
|-NewExpansionExpansion.cpp (APP implementation class)
```
