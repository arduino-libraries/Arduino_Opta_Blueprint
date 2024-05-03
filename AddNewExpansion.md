# Add a new device to Opta Blueprint

## CommonCfg.h
Add new expansion type to ExpansionType_t

## Controller.cpp
Add new expansion type to getExpansionPtr()

## OptaBlue.h
Add new Expansion file include 

## OptaDevicesCfg.h
Add new Expansion Configuration file


## add new expansion FW
OptaUnoR4Display.h
OptaUnoR4Display.cpp
OptaUnoR4DisplayCfg.h
+ sketch in firmware folder

## add new Expansion subclass
R4DisplayExpansion.cpp
R4DisplayExpansion.h
R4DisplayAddress.h

## add file with definition common between FW and application
+ CommonOptaUnoR4Display.h

## add opta  uno r4 display in OptaDeviceInclude
This is necessary to have the possibility
to "segregate" the FW into a subfolder



