NFC Reader Library v4.010.03.001609 for Linux
===========================

This package contains the complete NFC Reader Library with generic porting layers for Linux.
It is intended to run on any Linux system, but prepared to run on the Raspberry Pi with
Raspbian Linux.
To change the platform, the hardware pin configuration in the following file need to be aligned:

NfcReaderLibrary_V4.010.03.001609_Linux
└── linux 
    ├── comps
    │   └── phbalReg
    └── shared
        └── phhwConfig.h                         // lines 28-34, 65-76 (PN512), 86-102 (CLRC663), 111-119 (PN5180)

The software is prepared to either work with the EXPLORE-NFC (PN512) or with the PNEV5180B board
from the kit OM25180FDK or with the CLEV663B.


Preparing the hardware
---------------

EXPLORE-NFC:
The EXPLORE-NFC can be used as is. No modification is required.

PNEV5180B v1.0:
The following resistors need to be removed:
On the top side:
R034 SCL
R033 SSEL
R036 MISO
R039 MOSI
On the bottom side
Remove all connections to MCU (IRQ, BUSY) via JP23  (JUMPER_4POL)

PNEV5180B v2.0:
The following resistors need to be removed:
R233 (BUSY)
R235 (IRQ)
R239 (SCK)
R240 (SSEL)
R241 (MISO)
R242 (MOSI)

CLEV663B v3.0:
In order to activate the interrupt connection, please remove the 0-Ohm resistor from R35
and solder it to R34.

Connecting the hardware
---------------

EXPLORE-NFC:
Just plug it onto the GPIO header of the Raspberry Pi as described in the Start-up Guide of the
Explore-NFC

PNEV5180B:
To power the PNEV5180B, connect the USB cable from the Raspberry Pi to the PNEV5180B.
From the break out headers the following pins need to be connected to the Raspberry Pi:

PN5180 / Raspberry Pi (connector P1):
MOSI    19
MISO    21
SCK     23
nCS     24

BUSY    22
IRQ     16
nRESET  26
GND      6

CLEV663B:
To power the CLEV663B, connect the USB cable from the Raspberry Pi to the CLEV663B.
From the LPCXpresso headers the following pins need to be connected to the Raspberry Pi:

Pin CLEV663B  Pin Raspberry Pi  Name
1             6                 GND
2             2                 +5V
5             19                MOSI
6             21                MISO
7             23                SCK
8             24                SSEL
28            1                 +3,3V
39            18                PDOWN
46            13                IFSEL0
47            15                IFSEL1
53            16                IRQ


Getting started
---------------

Build dependencies:
cmake, gcc, make

On Raspbian Jessie, only cmake needs to be installed with the command

```
sudo apt-get install cmake
```

Building the project:
---------------

Change into the `build` folder with

```
cd build
```

and execute the following commands:
```
cmake ..
make
```

For a debug build:
```
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

In order to build the BAL for the BAL kernel space module:
```
cmake -DUSE_KERNEL_SPACE_BAL=1 ..
make
```

After building, you will find the example applications in the folder ../build/examples/.
To run them change into the right sub-directory and execute them with the command
./NfcrdlibEx*.


NfcReaderLibrary_V4.010.03.001609_Linux
├── build
│   ├── CMakeCache.txt
│   │
│   ├── examples
│   │   ├── NfcrdlibEx1_BasicDiscoveryLoop              // Contains example 1
│   │   │   ├── NfcrdlibEx1_BasicDiscoveryLoopRC663     // Ex1 for CLRC663
│   │   │   ├── NfcrdlibEx1_BasicDiscoveryLoopPN512     // Ex1 for PN512
│   │   │   └── NfcrdlibEx1_BasicDiscoveryLoopPN5180    // Ex1 for PN5180
        .
        .
        .
        .
├── CMakeLists.txt
├── examples                                            // Contains source code of the examples
│   ├── CMakeLists.txt
│   ├── NfcrdlibEx1_BasicDiscoveryLoop
│   │   ├── CMakeLists.txt
│   │   ├── intfs
│   │   │   └── cards.h
│   │   ├── NfcrdlibEx1_BasicDiscoveryLoop.c
│   │   ├── Readme.txt
│   │   └── src
│   │       └── cards.c
│   ├── NfcrdlibEx2_AdvancedDiscoveryLoop
│   │   ├── CMakeLists.txt
│   │   ├── intfs
│   │   │   └── cards.h
│   │   ├── Makefile
│   │   ├── NfcrdlibEx2_AdvancedDiscoveryLoop.c
│   │   ├── Readme.txt
│   │   └── src
│   │       └── cards.c
│   ├── NfcrdlibEx3_NFCForum
│   │   ├── CMakeLists.txt
│   │   ├── intfs
│   │   │   ├── cards.h
│   │   │   └── NfcrdlibEx3_NFCForum.h
│   │   ├── NfcrdlibEx3_NFCForum.c
│   │   ├── Readme.txt
│   │   └── src
│   │       └── cards.c
│   ├── NfcrdlibEx4_MIFAREClassic
│   │   ├── CMakeLists.txt
│   │   ├── Makefile
│   │   ├── NfcrdlibEx4_MIFAREClassic.c
│   │   └── Readme.txt
│   ├── NfcrdlibEx5_ISO15693
│   │   ├── CMakeLists.txt
│   │   ├── Makefile
│   │   ├── NfcrdlibEx5_ISO15693.c
│   │   └── Readme.txt
│   ├── NfcrdlibEx6_EMVCo_Loopback
│   │   ├── CMakeLists.txt
│   │   ├── NfcrdlibEx6_EMVCo_Loopback.c
│   │   └── Readme.txt
│   ├── NfcrdlibEx7_EMVCo_Polling
│   │   ├── CMakeLists.txt
│   │   ├── Makefile
│   │   ├── NfcrdlibEx7_EMVCo_Polling.c
│   │   └── Readme.txt
│   ├── NfcrdlibEx8_HCE_T4T
│   │   ├── CMakeLists.txt
│   │   ├── intfs
│   │   │   └── NfcrdlibEx8_HCE_T4T.h
│   │   ├── Makefile
│   │   ├── NfcrdlibEx8_HCE_T4T.c
│   │   └── Readme.txt
│   └── NfcrdlibEx9_NTagI2C
│       ├── CMakeLists.txt
│       ├── intfs
│       │   └── phhwConfig.h
│       ├── Makefile
│       ├── NfcrdlibEx9_NTagI2C.c
│       └── Readme.txt
├── linux                                               // Contains the Linux bindings
│   ├── CMakeLists.txt
│   ├── comps
│   │   ├── phbalReg
│   │   │   └── src
│   │   │       └── Linux
│   │   │           ├── phbalReg_Stub.c
│   │   │           ├── phbalReg_Stub_Utils.c
│   │   │           └── phbalReg_Stub_Utils.h
│   │   ├── phOsal
│   │   │   └── src
│   │   │       └── Posix
│   │   │           ├── phOsal_Posix.c
│   │   │           └── phOsal_Posix_Int.h
│   │   └── phPlatform
│   │       └── src
│   │           └── Posix
│   │               ├── phPlatform.c
│   │               └── phPlatform.h
│   ├── intfs
│   │   └── ph_NxpBuild_App.h
│   └── shared
│       ├── phhwConfig.c
│       └── phhwConfig.h
├── nxprdlib                                            // Contains the comlpete NFC Reader Library
│   └── NxpRdLib                                           unmodified
│       ├── comps
│       ├── docs
│       │   ├── 14_user_doc
│       │   │   ├── NXP NFC Reader Library.chm
│       │   │   └── NXP_SLDA.pdf
│       │   └── 22_rel_doc
│       │       └── NXP Reader Library Release Note.pdf
│       ├── intfs
│       └── types
└── README.md


Known problems and limitationes
---------------

* 3 compiler warnings in file
  nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180.c
  are present and will be fixed in the next release.
* 1 compiler warning in file
  nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180_Int.c
  is present and will be fixed in the next release.
