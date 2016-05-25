link_directories(${NXPRDLIBLINUX_BUILD_DIR}/linux)

set(sources NfcrdlibEx8_HCE_T4T.c)
set(includes ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/types ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib 
${NXPRDLIBLINUX_SOURCE_DIR}/linux/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/example/NfcrdlibEx8_HCE_T4T/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phbalReg/src/Linux ${NXPRDLIBLINUX_SOURCE_DIR}/linux/shared intfs)
set( definitions -D NXPBUILD_CUSTOMER_HEADER_INCLUDED -D DEBUG -D NATIVE_C_CODE -D LINUX )

add_executable (NfcrdlibEx8_HCE_T4TPN512 ${sources})
add_executable (NfcrdlibEx8_HCE_T4TPN5180 ${sources})

target_link_libraries (NfcrdlibEx8_HCE_T4TPN512 LINK_PUBLIC NxpRdLibLinuxPN512 pthread rt)
target_link_libraries (NfcrdlibEx8_HCE_T4TPN5180 LINK_PUBLIC NxpRdLibLinuxPN5180 pthread rt)

target_compile_definitions(NfcrdlibEx8_HCE_T4TPN512 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC523)
target_compile_definitions(NfcrdlibEx8_HCE_T4TPN5180 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_PN5180)
target_include_directories(NfcrdlibEx8_HCE_T4TPN512 PUBLIC ${includes})
target_include_directories(NfcrdlibEx8_HCE_T4TPN5180 PUBLIC ${includes})

add_definitions(-std=gnu99)
