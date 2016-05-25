link_directories(${NXPRDLIBLINUX_BUILD_DIR}/linux)

set(sources NfcrdlibEx3_NFCForum.c src/cards.c)
set(includes ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib/types ${NXPRDLIBLINUX_SOURCE_DIR}/nxprdlib/NxpRdLib 
${NXPRDLIBLINUX_SOURCE_DIR}/linux/intfs ${NXPRDLIBLINUX_SOURCE_DIR}/examples/NfcrdlibEx3_NFCForum/intfs {NXPRDLIBLINUX_SOURCE_DIR}/linux/comps/phbalReg/src/Linux ${NXPRDLIBLINUX_SOURCE_DIR}/linux/shared intfs)
set( definitions -D NXPBUILD_CUSTOMER_HEADER_INCLUDED -D DEBUG -D NATIVE_C_CODE -D LINUX )

add_executable (NfcrdlibEx3_NFCForumPN512 ${sources})
add_executable (NfcrdlibEx3_NFCForumPN5180 ${sources})

target_link_libraries (NfcrdlibEx3_NFCForumPN512 LINK_PUBLIC NxpRdLibLinuxPN512 pthread rt)
target_link_libraries (NfcrdlibEx3_NFCForumPN5180 LINK_PUBLIC NxpRdLibLinuxPN5180 pthread rt)

target_compile_definitions(NfcrdlibEx3_NFCForumPN512 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_RC523)
target_compile_definitions(NfcrdlibEx3_NFCForumPN5180 PUBLIC ${definitions} -D NXPBUILD__PHHAL_HW_PN5180)
target_include_directories(NfcrdlibEx3_NFCForumPN512 PUBLIC ${includes})
target_include_directories(NfcrdlibEx3_NFCForumPN5180 PUBLIC ${includes})

add_definitions(-std=gnu99)
