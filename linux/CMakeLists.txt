
set(sources
comps/phbalReg/src/Linux/phbalReg_Stub_Utils.c comps/phOsal/src/Posix/phOsal_Posix.c comps/phPlatform/src/Posix/phPlatform.c
shared/phhwConfig.c

../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/phacDiscLoop.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_A.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_B.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_F.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_I18000p3m3.c
../nxprdlib/NxpRdLib/comps/phacDiscLoop/src/Sw/phacDiscLoop_Sw_Int_V.c
../nxprdlib/NxpRdLib/comps/phalFelica/src/phalFelica.c
../nxprdlib/NxpRdLib/comps/phalFelica/src/Sw/phalFelica_Sw.c
../nxprdlib/NxpRdLib/comps/phalI15693/src/phalI15693.c
../nxprdlib/NxpRdLib/comps/phalI15693/src/Sw/phalI15693_Sw.c
../nxprdlib/NxpRdLib/comps/phalI18000p3m3/src/phalI18000p3m3.c
../nxprdlib/NxpRdLib/comps/phalI18000p3m3/src/Sw/phalI18000p3m3_Sw.c
../nxprdlib/NxpRdLib/comps/phalI18000p3m3/src/Sw/phalI18000p3m3_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phalMfc/src/phalMfc.c
../nxprdlib/NxpRdLib/comps/phalMfc/src/phalMfc_Int.c
../nxprdlib/NxpRdLib/comps/phalMfc/src/Sw/phalMfc_Sw.c
../nxprdlib/NxpRdLib/comps/phalMfdf/src/phalMfdf.c
../nxprdlib/NxpRdLib/comps/phalMfdf/src/phalMfdf_Int.c
../nxprdlib/NxpRdLib/comps/phalMfdf/src/Sw/phalMfdf_Sw.c
../nxprdlib/NxpRdLib/comps/phalMfdf/src/Sw/phalMfdf_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phalMful/src/phalMful.c
../nxprdlib/NxpRdLib/comps/phalMful/src/phalMful_Int.c
../nxprdlib/NxpRdLib/comps/phalMful/src/Sw/phalMful_Sw.c
../nxprdlib/NxpRdLib/comps/phalSli/src/phalSli.c
../nxprdlib/NxpRdLib/comps/phalSli/src/Sw/phalSli_Sw.c
../nxprdlib/NxpRdLib/comps/phalT1T/src/phalT1T.c
../nxprdlib/NxpRdLib/comps/phalT1T/src/Sw/phalT1T_Sw.c
../nxprdlib/NxpRdLib/comps/phalTop/src/phalTop.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw_Int_T1T.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw_Int_T2T.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw_Int_T3T.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw_Int_T4T.c
../nxprdlib/NxpRdLib/comps/phalTop/src/Sw/phalTop_Sw_Int_T5T.c
../nxprdlib/NxpRdLib/comps/phbalReg/src/phbalReg.c
../nxprdlib/NxpRdLib/comps/phceT4T/src/phceT4T.c
../nxprdlib/NxpRdLib/comps/phceT4T/src/Sw/phceT4T_Sw.c
../nxprdlib/NxpRdLib/comps/phceT4T/src/Sw/phceT4T_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/phhalHw.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180_Instr.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180_Int.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Pn5180/phhalHw_Pn5180_Wait.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc523/phhalHw_Rc523.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc523/phhalHw_Rc523_Cmd.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc523/phhalHw_Rc523_Int.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc523/phhalHw_Rc523_Wait.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc663/phhalHw_Rc663.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc663/phhalHw_Rc663_Cmd.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc663/phhalHw_Rc663_Int.c
../nxprdlib/NxpRdLib/comps/phhalHw/src/Rc663/phhalHw_Rc663_Wait.c
../nxprdlib/NxpRdLib/comps/phKeyStore/src/phKeyStore.c
../nxprdlib/NxpRdLib/comps/phKeyStore/src/Sw/phKeyStore_Sw.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/phlnLlcp.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw_Mac.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw_Transport.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw_Transport_Connection.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Sw_Transport_Connectionless.c
../nxprdlib/NxpRdLib/comps/phlnLlcp/src/Sw/phlnLlcp_Timers.c
../nxprdlib/NxpRdLib/comps/phLog/src/phLog.c
../nxprdlib/NxpRdLib/comps/phnpSnep/src/phnpSnep.c
../nxprdlib/NxpRdLib/comps/phnpSnep/src/Sw/phnpSnep_Sw.c
../nxprdlib/NxpRdLib/comps/phpalEpcUid/src/phpalEpcUid.c
../nxprdlib/NxpRdLib/comps/phpalEpcUid/src/Sw/phpalEpcUid_Sw.c
../nxprdlib/NxpRdLib/comps/phpalFelica/src/phpalFelica.c
../nxprdlib/NxpRdLib/comps/phpalFelica/src/Sw/phpalFelica_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p3a/src/phpalI14443p3a.c
../nxprdlib/NxpRdLib/comps/phpalI14443p3a/src/Sw/phpalI14443p3a_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p3b/src/phpalI14443p3b.c
../nxprdlib/NxpRdLib/comps/phpalI14443p3b/src/Sw/phpalI14443p3b_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4/src/phpalI14443p4.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4/src/Sw/phpalI14443p4_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4a/src/phpalI14443p4a.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4a/src/Sw/phpalI14443p4a_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4mC/src/phpalI14443p4mC.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4mC/src/Sw/phpalI14443p4mC_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI14443p4mC/src/Sw/phpalI14443p4mC_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phpalI18000p3m3/src/phpalI18000p3m3.c
../nxprdlib/NxpRdLib/comps/phpalI18000p3m3/src/Sw/phpalI18000p3m3_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI18092mPI/src/phpalI18092mPI.c
../nxprdlib/NxpRdLib/comps/phpalI18092mPI/src/Sw/phpalI18092mPI_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI18092mPI/src/Sw/phpalI18092mPI_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phpalI18092mT/src/phpalI18092mT.c
../nxprdlib/NxpRdLib/comps/phpalI18092mT/src/Sw/phpalI18092mT_Sw.c
../nxprdlib/NxpRdLib/comps/phpalI18092mT/src/Sw/phpalI18092mT_Sw_Int.c
../nxprdlib/NxpRdLib/comps/phpalMifare/src/phpalMifare.c
../nxprdlib/NxpRdLib/comps/phpalMifare/src/Sw/phpalMifare_Sw.c
../nxprdlib/NxpRdLib/comps/phpalSli15693/src/phpalSli15693.c
../nxprdlib/NxpRdLib/comps/phpalSli15693/src/Sw/phpalSli15693_Sw.c
../nxprdlib/NxpRdLib/comps/phTools/src/phTools.c
../nxprdlib/NxpRdLib/comps/phTools/src/phTools_Q.c

)
if(NOT DEFINED USE_KERNEL_SPACE_BAL)
   set(USE_KERNEL_SPACE_BAL 0)
endif(NOT DEFINED USE_KERNEL_SPACE_BAL)
if(USE_KERNEL_SPACE_BAL)
   list(APPEND sources comps/phbalReg/src/Linux/phbalReg_Stub_Kernel.c)
else(USE_KERNEL_SPACE_BAL)
   list(APPEND sources comps/phbalReg/src/Linux/phbalReg_Stub.c)
endif(USE_KERNEL_SPACE_BAL)

set( definitions -D NXPBUILD_CUSTOMER_HEADER_INCLUDED -D DEBUG -D NATIVE_C_CODE -D LINUX )
set( includes ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/types
 ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/comps/phbalReg/src/Stub
 ${NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phbalReg/src/Linux
 ${NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phPlatform/src/Posix
 ${NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phOsal/src/Posix
 ${NXPRDLIBLINUX_SOURCE_DIR}/linux/intfs )

add_library (NxpRdLibLinuxPN512 STATIC ${sources})
add_library (NxpRdLibLinuxPN5180 STATIC ${sources})
add_library (NxpRdLibLinuxRC663 STATIC ${sources})

install(TARGETS NxpRdLibLinuxPN512 NxpRdLibLinuxPN5180 NxpRdLibLinuxRC663
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib/static)
        
install(DIRECTORY ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib DESTINATION include/nxprdlib
        FILES_MATCHING PATTERN "*.h")
        
install(DIRECTORY ${NXPRDLIBLINUX_SOURCE_DIR}/linux DESTINATION include/nxprdlib
        FILES_MATCHING PATTERN "*.h")

target_compile_definitions(NxpRdLibLinuxPN512 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC523)
target_compile_definitions(NxpRdLibLinuxPN5180 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_PN5180)
target_compile_definitions(NxpRdLibLinuxRC663 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC663)

target_include_directories(NxpRdLibLinuxPN512 PUBLIC ${includes})
target_include_directories(NxpRdLibLinuxPN5180 PUBLIC ${includes})
target_include_directories(NxpRdLibLinuxRC663 PUBLIC ${includes})

add_definitions(-std=gnu99 -pthread)
