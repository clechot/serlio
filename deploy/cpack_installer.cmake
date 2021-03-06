# setup wix installer
include(${CMAKE_CURRENT_LIST_DIR}/cpack_common.cmake)

# Windows installer needs slightly different folder layout
set(INSTALL_FOLDER_PREFIX "Contents/${maya_VERSION_MAJOR}")

set(CPACK_GENERATOR WIX)

# UPGRADE_GUID. This GUID must remain constant, it helps the installer
# to identify different (previous) installations of the same software.
# The GUID must be different for different Maya versions to allow for
# side-by-side installations.
set(CPACK_WIX_UPGRADE_GUID "9B170749-ECDF-4F66-9E2F-C9045727${maya_VERSION_MAJOR}")

set(CPACK_WIX_PRODUCT_GUID "*")

# This sets the path to the license file explicitly. If not set, it
# defaults to CPACK_RESOURCE_FILE_LICENSE, which we could use instead.
set(CPACK_WIX_LICENSE_RTF "${CMAKE_CURRENT_LIST_DIR}/resources/license.rtf")

# (1) Installer Icon
set(CPACK_WIX_PRODUCT_ICON "${CMAKE_CURRENT_LIST_DIR}/resources/serlio.ico")
#
# (2) Banner, visible on all Wizard pages except first and last. Must be 493x58 pixels.
set(CPACK_WIX_UI_BANNER "${CMAKE_CURRENT_LIST_DIR}/resources/banner.png")
#
# (3) Dialog background, appears on first and last page of the Wizard. Must be 493x312 pixels.
set(CPACK_WIX_UI_DIALOG "${CMAKE_CURRENT_LIST_DIR}/resources/dialog.png")

# XML doesn't like naked '&'.
set(PACKAGE_VENDOR_ESCAPED "Esri R&amp;D Center Zurich")
set(CPACK_WIX_SKIP_PROGRAM_FOLDER true)

# Fill in the PackageContents template
configure_file(${CMAKE_CURRENT_LIST_DIR}/PackageContents.xml.in ${PROJECT_BINARY_DIR}/PackageContents.xml)

# We need to overwrite a couple of variables from cpack_common because slightly different values are required.
set(CPACK_PACKAGE_INSTALL_DIRECTORY   "C:/ProgramData/Autodesk/ApplicationPlugins/serlio-${SRL_VERSION_MMP_PRE}")

# Careful: CPACK_PACKAGE_NAME is also used in the PackageContents.xml template above.
# Make sure that this line always appears after the 'configure_file' directive.
set(CPACK_PACKAGE_NAME   "Serlio for Maya ${maya_VERSION_MAJOR}")
# We need to set the CPACK_PACKAGE_VERSION to a format that MSI understands (MAJOR.MINOR.PATCH).
set(CPACK_PACKAGE_VERSION   ${SRL_VERSION_MMP})

# Add it to the WiX installer.
install(FILES ${PROJECT_BINARY_DIR}/PackageContents.xml DESTINATION "/")

# Inject patch to force root drive to C:\ (see https://stackoverflow.com/a/52561165/1097883)
set(CPACK_WIX_PATCH_FILE "${CMAKE_CURRENT_LIST_DIR}/wix_patches/force_rootdrive.wxpatch")