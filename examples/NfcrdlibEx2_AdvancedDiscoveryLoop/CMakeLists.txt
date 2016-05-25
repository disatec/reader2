link_directories(${NXPRDLIBLINUX_BUILD_DIR}/linux)

set(sources NfcrdlibEx2_AdvancedDiscoveryLoop.c src/cards.c)
set(includes ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/types ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib 
${NXPRDLIBLINUX_SOURCE_DIR}/linux/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/NfcrdlibEx2_AdvancedDiscoveryLoop/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phbalReg/src/Linux ${NXPRDLIBLINUX_SOURCE_DIR}/linux/shared intfs)
set( definitions -D NXPBUILD_CUSTOMER_HEADER_INCLUDED -D DEBUG -D NATIVE_C_CODE -D LINUX )

add_executable (NfcrdlibEx2_AdvancedDiscoveryLoopPN512 ${sources})
add_executable (NfcrdlibEx2_AdvancedDiscoveryLoopPN5180 ${sources})
add_executable (NfcrdlibEx2_AdvancedDiscoveryLoopRC663 ${sources})


target_link_libraries (NfcrdlibEx2_AdvancedDiscoveryLoopPN512 LINK_PUBLIC NxpRdLibLinuxPN512 pthread rt)
target_link_libraries (NfcrdlibEx2_AdvancedDiscoveryLoopPN5180 LINK_PUBLIC NxpRdLibLinuxPN5180 pthread rt)
target_link_libraries (NfcrdlibEx2_AdvancedDiscoveryLoopRC663 LINK_PUBLIC NxpRdLibLinuxRC663 pthread rt)


target_compile_definitions(NfcrdlibEx2_AdvancedDiscoveryLoopPN512 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC523)
target_compile_definitions(NfcrdlibEx2_AdvancedDiscoveryLoopPN5180 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_PN5180)
target_compile_definitions(NfcrdlibEx2_AdvancedDiscoveryLoopRC663 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC663)
target_include_directories(NfcrdlibEx2_AdvancedDiscoveryLoopPN512 PUBLIC ${includes})
target_include_directories(NfcrdlibEx2_AdvancedDiscoveryLoopPN5180 PUBLIC ${includes})
target_include_directories(NfcrdlibEx2_AdvancedDiscoveryLoopRC663 PUBLIC ${includes})

add_definitions(-std=gnu99)
