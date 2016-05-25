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
* Readme for NFC Forum example (NfcrdlibEx3_NFCForum).
* $Author: nxp69431 $
* $Revision: 1.0 $
* $Date: 2015-07-13 16:30:30 +0530 (Monday, 13th July 2014) $
*/
==================================================================================
Readme - Software for the PNEV512B Blueboard 1.5 and PN5180 TFBGA64 performance
board v1.0 
==================================================================================

1. DOCUMENT PURPOSE:
    This document describes the steps to be followed to execute the example package
    NfcrdlibEx3_NFCForum as well as known problems and restrictions.

2. DESCRIPTION:
    This example will demonstrate both Peer to Peer (P2P) and Reader functions
    to detect type A, type B, type F tags, type V tags and 18000p3m3 tags (subject
    to reader hardware limitations - Example, only when built with PNEV512B Type V
    and ISO 18000p3m3 tags are not supported).

    If any P2P device is brought in proximity then this example will go into
    P2P Mode. When SNEP Client is enabled then this example performs SNEP PUT onto 
    the Peer device. By default the configured SNEP PUT message is "www.nxp.com".

    To demonstrate SNEP Server, the SNEP Client needs to be disabled as no control
    can be provided with boards as incase of mobiles (Just like the Touch-to-beam
    feature on Mobiles). The maximum length of SNEP PUT from peer device that can
    be received by SNEP Server with this example is 1KB (1024 Bytes).

    If any NFC cards/devices (Type A, Type B or Type F) are brought in proximity 
    then this example will behave as a reader and will detect and print the NFC 
    technology type detected along with NDEF message from the tag if present.

3. RESTRICTIONS:
    a) The Software is restricted to list of supported NFC Reader Boards mentioned
       in point (10), connected to Contex M3 LPC1769 micro-controller from NXP.
       The APIs are intended for NXP contact-less reader ICs only, as such the APIs
       are not to be ported to any technology from any other vendor.
    b) NXP will not support porting to any other vendor platform. 
    c) This software project requires NFC Reader Library version 3.092.12.001547
       or later.
    d) To compile and flash this project into the controller, you need to import
       this project using NXP LPCXpresso v7.9.x.

4. CONFIGURATIONS in Example-3:
    The example application is written to detect and activate one tag of supported
    technologies. Listen mode is supported only for PN512 & PN5180.
    ph_NxpBuild_Lpc.h contains the components that need to be enabled for
    compilation in this project. If switching between suppported NFC Reader boards
    mention in point (10), user is adviced to include the appropriate define in
    file metioned in point (9)(f).

5. PACKAGE CONTENTS: 
    a) intf\
            1. intf\NfcrdlibEx3_NFCForum.h
                   Example header file.
            2. intf\ph_NxpBuild_App.h
                   Reader library build configuration file.
            3. intf\phhwConfig.h
                   Hardware configuration header file.
            4. intf\cards.h
                   Common utility header file.
    b) src\
            1. src\phhwConfig.c
                   Hardware configuration file.
            2. src\cards.c
                   Common utility functions file.
    c) .cproject
            LPCXpresso project configuration file.
    d) .project
            LPCXpresso configuration file.
    e) NfcrdlibEx3_NFCForum.c
            Main example file.
    f) Readme.txt 
            This readme file. 

6.SOFTWARE CONFIGURATIONS:
    a) Reader Library layers: Only reader library layers mentioned in the header
       file ph_NxpBuild_App.h are enabled in build for this project.
    b) Listen Mode: In discovery loop listen mode, active listen or passive listen 
       can be selectively enabled/disabled by defining "ACTIVE_MODE" and
       "PASSIVE_MODE" respectively.
    c) Poll Mode: In discovery loop poll mode, active poll or passive poll can be
       selectively enabled/disabled by defining "ACTIVE_MODE" and "PASSIVE_MODE"
       respectively.
    d) Active Mode: To enable Active Mode. If Disabled both Active Poll and Active
       Listen discovery loop configuration will be disabled.
    e) Passive Mode: To enable Passive Mode. If Disabled both Passive Poll and
       Passive Listen discovery loop configuration will be disabled.
    f) SNEP Client: To enable SNEP Client functionality i.e. to perform SNEP PUT
       onto remote peer device (Mobile/Board).
    g) SNEP Server: To enable SNEP Server functionality i.e. to send SNEP PUT
       Message from remote peer device (Mobile/Board).

7. HARDWARE CONFIGURATIONS:
    Before using this application ensure the following hardware changes are done
    for different reader IC's.
    a) For PNEV512B board change has been done on the LPC1769 board - GPIO 2.13
       (Pin #27) of LPC1769 should be connected to IRQ PIN (#26) of PNEV512 board.
    b) For PNEV5180B two USB ports available Mini+Macro type both serve same
       operation. So connection to any one of it will work. Jumpers settings on
       board are following JP4 Connection of pin2-3, JP49 connection of pin2-3,
       PH3 connection of pin1-2. No other specific hardware change for IRQ PIN.

8. MANDATORY MATERIALS, not included:
    a) LPCXpresso IDE as described on the web site:
        http://www.lpcware.com/lpcxpresso
    b) LPCXpresso LPC1769 development board with one of supported NFC Reader boards
       mentioned in point(10).

9. STEPS TO FOLLOW:
    Please note - If you have not done the hardware changes as mentioned in the
    release notes at docs/22_rel_doc for PNEV512B Blueboard 1.5 the example will
    not work with PNEV512B. Please do the steps as given in the release notes.
    Step 1: Opening the package in LPCXpresso
    a) For running this application, connect the system comprising of LPC1769 and
       one of NFC Reader boards supported in point(10) to the PC.
    b) Create a new LpcXpresso workspace and then import this project along with
       FreeRTOS and Lpc1769 Project to get it compiled.
    c) Build the project.
    d) Start the "Debug" session of LPCXpresso which will first flash the
       executable and start the task.
    e)  i. When the application is running, if any NFC cards/devices (Type A, Type B
           or Type F) are brought in proximity then this example will behave as a
           reader and will detect and report the NFC technology types detected along
           with NDEF message from the tag if present.
       ii. If any P2P device is brought in proximity then this example will
           dynamically switch between Initiator or Target mode and performs SNEP PUT
           if both Poll and Listen Modes are enabled. If SNEP Client is disabled then
           Peer P2P device can perform SNEP PUT operation which will be received by
           this example.
    f) Please ensure that you change the #defines to either use Rc523 or PN5180 HAL
       in ph_NxpBuild_App.h with the intfs folder of the example application
       depending on the IC that is used.   
           Eg. for PN5180 "#define NXPBUILD__PHHAL_HW_PN5180"
    g) To control build behavior of components please refer ph_NxpBuild_App.h.

10. LIST OF SUPPORTED NFC READER BOARDS:
    a) PNEV512B Blueboard 1.5
    b) PN5180 TFBGA64 performance board v1.0   