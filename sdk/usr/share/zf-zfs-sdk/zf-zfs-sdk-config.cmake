#
# should return something like
#  /usr/share/zfzfssdk
#       -------------- -> this part, we think, we know how it's named...
#  ---- -> this is the part we call PREFIX and need to build the rest of the
#          directories

# like ${PREFIX}/include
# and  ${PREFIX}/lib
# and  ${PREFIX}/bin
# and  ${PREFIX}/share
get_filename_component(zf-zfs-sdk_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

get_filename_component(zf-zfs-sdk_INCLUDE_DIRS
    ${zf-zfs-sdk_CMAKE_DIR}/../../include/zf ABSOLUTE)
get_filename_component(zf-zfs-sdk_LIBRARY_DIRS
    ${zf-zfs-sdk_CMAKE_DIR}/../../lib ABSOLUTE)

include("${zf-zfs-sdk_CMAKE_DIR}/zf-zfs-sdk-targets.cmake")

# add all other libraries SDK libs here...
set(zf-zfs-sdk_LIBRARIES zfs zftools zfnav zfssim zfxyz)
