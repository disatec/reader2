/*
*         Copyright (c), NXP Semiconductors Bangalore / India
*
*                     (C)NXP Semiconductors
*       All rights are reserved. Reproduction in whole or in part is 
*      prohibited without the written consent of the copyright owner.
*  NXP reserves the right to make changes without notice at any time.
* NXP makes no warranty, expressed, implied or statutory, including but
* not limited to any implied warranty of merchantability or fitness for any
* particular purpose, or that the use will not infringe any third party patent,
* copyright or trademark. NXP must not be liable for any loss or damage
*                          arising from its use.
*/

/** \file
* Readme for card emulation example (NfcrdlibEx8_HCE_T4T).
* $Author: nxp66431 $
* $Revision: 1.0 $
* $Date: 2014-08-08 16:30:30 +0530 (Friday, 8th August 2014) $
*/
================================================================================
                           CARD EMULATION EXAMPLE
================================================================================

DOCUMENT PURPOSE

    This document describes the steps to be followed to execute the example 
    package NfcrdlibEx8_HCE_T4T as well as known problems and restrictions.

DESCRIPTION

    This example will demonstrate card emulation of type 4A tag (T4AT).
 
    If any reader device is brought in proximity then this example will go into
    card emulation mode. In card emulation mode this allows both reading and 
    writing of NDEF message by reader. By default the configured NDEF message 
    is "www.nxp.com". The maximum NDEF length the reader can write is limited 
    by NDEF file size used in example (default configured as 1024 bytes).

    This example can also be used for running ISO 10373-6 PICC test suite. To 
    run with PICC test suite, set ISO_10373_6_PICC_TEST_BENCH macro to 1 in 
    NfcrdlibEx8_HCE_T4T.h and load the Micropross test bench configuration file 
    ISO_10373_6_PICC_Test_Bench_Config.mpecc in Micropross PICC test suite. 
    Enabling ISO_10373_6_PICC_TEST_BENCH macro will enable the extra lines of 
    code needed for testing.

SUPPORTED NFC READER BOARDS:

    a) PNEV512B Blueboard v1.5
    b) PNEV5180B v1.0

RESTRICTIONS:

    a) The Software is restricted to list of supported NFC Reader Boards
       mentioned above, connected to Cortex M3 LPC1769 micro-controller from 
       NXP. The APIs are intended for NXP contact-less reader ICs only, as such 
       the APIs are not to be ported to any technology from any other vendor.
    b) NXP will not support porting to any other vendor platform. 
    c) This software project requires NFC Reader Library version 3.092.11.001547
       or higher.
    d) To compile and flash this project into the controller, you need to import 
       this project using NXP LPCXpresso v7.9.x.
    e) To execute the ISO PICC test bench using the configurations provided,
       Micropross MP500 TCL3 tool with ISO 10373-6 PICC test suite is required.

PACKAGE CONTENTS:

    a) intf\
            1. intf\NfcrdlibEx8_HCE_T4T.h
                   Example header file.
            2. intf\ph_NxpBuild_App.h
                   Reader library build configuration file.
            3. intf\phhwConfig.h
                   Hardware configuration header file.
    b) src\
            1. src\phhwConfig.c
                   Hardware configuration file.
    c) .cproject
            LPCXpresso project configuration file.
    d) .project
            LPCXpresso configuration file.
    e) ISO_10373_6_PICC_Test_Bench_Config.mpecc
            Micropross ISO PICC test suite configuration file.
    f) NfcrdlibEx8_HCE_T4T.c
            Main example file.
    g) Readme.txt 
            This readme file. 

SOFTWARE CONFIGURATIONS:

    a) Reader Library layers: Only reader library layers mentioned in the header
       file ph_NxpBuild_App.h are enabled in build for this project.
    b) Listen Mode: In discovery loop listen mode, active listen is disabled and 
       only passive listen is enabled to support Type 4A card emulation. Poll 
       mode is not used.
    c) ISO 10373-6 PICC Test Bench: To run this project with ISO PICC test 
       bench, set ISO_10373_6_PICC_TEST_BENCH macro to 1 (NfcrdlibEx8_HCE_T4T.h)
       and load the configuration file ISO_10373_6_PICC_Test_Bench_Config.mpecc 
       in the Micropross PICC test suite.

HARDWARE CONFIGURATIONS:

    a) PN512

       Before using this application ensure the following hardware change has 
       been done on the LPC1769 board - GPIO 2.13 (Pin #27) of LPC1769 should 
       be connected to IRQ PIN (#26) of PNEV512 board.

    b) PNEV5180B

       The two USB ports available Mini+Macro type both serves same operation, 
       so connection to any one of it will work. Jumper settings on board are 
       following JP4 Connection of pin2-3, JP49 connection of pin2-3, PH3 
       connection of pin1-2. No other specific hardware change for IRQ PIN.

MANDATORY MATERIALS, not included:

    a) LPCXpresso IDE as described on the web site:
            http://www.lpcware.com/lpcxpresso
    b) LPCXpresso LPC1769 development board with one of supported NFC Reader 
       boards.

OPTIONAL TOOLS:

    a) To run PICC ISO 10373-6 test bench, Micropross MP500 TCL3 tool along with 
       PICC test suite is required.

STEPS TO FOLLOW:

    a) For running this application, connect the system comprising of LPC1769 
       and NFC reader board to the PC.
    b) Create a new LpcXpresso workspace and then import this project into the 
       workspace along with Lpc1769 and FreeRTOS projects.
    c) To control build behavior of components please refer ph_NxpBuild_App.h.
       Ensure that the correct HAL #define is specified in ph_NxpBuild_App.h 
       based on the NFC reader IC that is being used. For example, for PN5180 
       use "#define NXPBUILD__PHHAL_HW_PN5180".
    d) Build the project.
    e) Start the "Debug" session of LPCXpresso which will first flash the 
       executable and start the application.
    f) When the application is running, if any reader device is brought in 
       proximity then this example will behave like a Type 4A tag (in card 
       emulation mode). In card emulation mode this allows both reading and 
       writing of NDEF message by reader.
