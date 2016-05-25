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
* Readme.txt for getting the first-hand knowledge of how to use the Example with NFC NXP Reader library.
* $Author: NXP95127 $
* $Revision: 1.0 $
* $Date: 2015-06-01 $
*
*/
==================================================================================================
Readme - Software for the PNEV512B Blueboard 1.5, CLEV663B v3.0 and PN5180 TFBGA64 
performance board v1.0
==================================================================================================

1. DOCUMENT PURPOSE:
	This document describes the steps to be followed to execute NfcrdlibEx4_MIFAREClassic package as well as 
	known problems and restrictions.

2. DESCRIPTION:
	This example will load/configure Discovery loop with NFC profile via SetConfig.
	By using POLL mode of discovery loop, example will detect the MIFARE Classic cards and displays detected 
	tag information like UID, ATQA, SAK, Sector Authentication status, Block Read and Block Write status.
	This example will activate device at index zero whenever multiple cards are detected.

3. RESTRICTIONS:
	a) The Software is restricted to list of supported NFC Reader Boards mentioned in point (10), connected to Contex M3 LPC1769 micro-controller from NXP. 
	   The APIs are intended for NXP contact-less reader ICs only, as such the APIs are not to be ported to any technology from any other vendor.
	b) NXP will not support porting to any other vendor platform. 
	c) This software project requires NFC Reader Library version 3.092.07.001537 or later.
	d) To compile and flash this project into the controller, you need to import this project using NXP LPCXpresso v7.9.x.

4. CONFIGURATIONS in Example-4:
   The example application is developed to detect and activate MIFARE Classic card.
   ph_NxpBuild_Lpc.h contains the components that needs to be enabled for compilation in this project. 
   If switching between	suppported NFC Reader boards mention in point (10), user is adviced to include the appropriate define in file metioned in point (9)(f).

5. PACKAGE CONTENTS:
    a) intf\
            1. intf\ph_NxpBuild_App.h
                   Reader library build configuration file.
            2. intf\phhwConfig.h
                   Hardware configuration header file.
    b) src\
            1. src\phhwConfig.c
                   Hardware configuration file.       
    c) .cproject
            LPCXpresso project configuration file.
    d) .project
            LPCXpresso configuration file.
    e) NfcrdlibEx4_MIFAREClassic.c
            Main example file.
    f) Readme.txt 
            This readme file

6. MANDATORY MATERIALS, not included:
	a) LPCXpresso IDE as described on the web site:
		http://www.lpcware.com/lpcxpresso
	b) LPCXpresso LPC1769 development board with one of supported NFC Reader boards mentioned in point(10).

7. HARDWARE CONFIGURATIONS:
	Before starting the application present at NfcrdlibEx1_BasicDiscoveryLoop ensure the following hardware changes   
	a) For PNEV512B board change has been done on the LPC1769 board - GPIO 2.13 (Pin #27) of LPC1769 should be connected
	   to IRQ PIN (#26) of PNEV512 board. This is required for the system to function in Target mode.
	b) For CLEV663B board by default Pin 32 of CLEV663B will be connect to Ground via R35. Remove R35 and connect a 0 Ohm to R34. 
	   This will connect CLEV663B IRQ pin connect to LPC 1769 Port 2.12 pin, which is an external interrupt for LPC1769.   
	c) For PNEV5180B two USB ports available Mini+Macro type both serves same operation,so connection to any one of it will work.
	   Jumpers settings on board are following JP4 Connection of pin2-3, JP49 connection of pin2-3, PH3 connection of pin1-2.
	   No other specific hardware change for IRQ PIN.

8. SOFTWARE CONFIGURATION:
    The Software can be compiled for Cortex M3 LPC1769 micro-controller from NXP.

9. STEPS TO FOLLOW:
	Please note - If you have not done the hardware changes as mentioned in the release notes at docs/22_rel_doc for CLRC663 blueboard,
	the example will not work with CLRC663. Please do the steps as given in the release notes.
	a) For running this application, connect the system comprising of LPC1769 and one of NFC Reader boards supported in point(10) to the PC.
	b) Create a new LpcXpresso workspace and the import this project along with NxpNfcRdLib and Lpc1769 Project to get it compiled.
	c) Build the project
	d) Start the "Debug" session of LPCXpresso which will first flash the executable and start the task.
	e) Run the application and when you bring the MIFARE Classic in proximity, the example application will detect and displays the 
		detected card information on console.
	f) Please ensure that you change the #defines to use relevant HAL in ph_NxpBuild_App.h with the intfs folder of the example
	   application depending on the IC that is used.   eg. for PN5180 "#define NXPBUILD__PHHAL_HW_PN5180"
	g) To control build behavior of components please refer ph_NxpBuild_App.h.		
	h) Please use the MIFARE Classic Cards which has a KeyA value of {0xFF 0xFF 0xFF 0xFF 0xFF 0xFF} for sector 1 to get the successful authentication.	

10. LIST OF SUPPORTED NFC READER BOARDS:
	a) PNEV512B Blueboard 1.5
	b) CLEV663B v3.0
	c) PN5180 TFBGA64 performance board v1.0 