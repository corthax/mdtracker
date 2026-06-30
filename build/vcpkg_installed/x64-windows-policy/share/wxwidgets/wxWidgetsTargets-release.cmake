#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "wx::wxbase" for configuration "Release"
set_property(TARGET wx::wxbase APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxbase PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxbase32u.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxbase32u_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxbase )
list(APPEND _cmake_import_check_files_for_wx::wxbase "${_IMPORT_PREFIX}/lib//wxbase32u.lib" "${_IMPORT_PREFIX}/bin//wxbase32u_vc_x64_custom.dll" )

# Import target "wx::wxnet" for configuration "Release"
set_property(TARGET wx::wxnet APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxnet PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxbase32u_net.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxbase32u_net_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxnet )
list(APPEND _cmake_import_check_files_for_wx::wxnet "${_IMPORT_PREFIX}/lib//wxbase32u_net.lib" "${_IMPORT_PREFIX}/bin//wxbase32u_net_vc_x64_custom.dll" )

# Import target "wx::wxcore" for configuration "Release"
set_property(TARGET wx::wxcore APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxcore PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_core.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_core_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxcore )
list(APPEND _cmake_import_check_files_for_wx::wxcore "${_IMPORT_PREFIX}/lib//wxmsw32u_core.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_core_vc_x64_custom.dll" )

# Import target "wx::wxadv" for configuration "Release"
set_property(TARGET wx::wxadv APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxadv PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_adv.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_adv_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxadv )
list(APPEND _cmake_import_check_files_for_wx::wxadv "${_IMPORT_PREFIX}/lib//wxmsw32u_adv.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_adv_vc_x64_custom.dll" )

# Import target "wx::wxaui" for configuration "Release"
set_property(TARGET wx::wxaui APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxaui PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_aui.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_aui_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxaui )
list(APPEND _cmake_import_check_files_for_wx::wxaui "${_IMPORT_PREFIX}/lib//wxmsw32u_aui.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_aui_vc_x64_custom.dll" )

# Import target "wx::wxhtml" for configuration "Release"
set_property(TARGET wx::wxhtml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxhtml PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_html.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_html_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxhtml )
list(APPEND _cmake_import_check_files_for_wx::wxhtml "${_IMPORT_PREFIX}/lib//wxmsw32u_html.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_html_vc_x64_custom.dll" )

# Import target "wx::wxpropgrid" for configuration "Release"
set_property(TARGET wx::wxpropgrid APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxpropgrid PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_propgrid.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_propgrid_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxpropgrid )
list(APPEND _cmake_import_check_files_for_wx::wxpropgrid "${_IMPORT_PREFIX}/lib//wxmsw32u_propgrid.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_propgrid_vc_x64_custom.dll" )

# Import target "wx::wxribbon" for configuration "Release"
set_property(TARGET wx::wxribbon APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxribbon PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_ribbon.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_ribbon_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxribbon )
list(APPEND _cmake_import_check_files_for_wx::wxribbon "${_IMPORT_PREFIX}/lib//wxmsw32u_ribbon.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_ribbon_vc_x64_custom.dll" )

# Import target "wx::wxrichtext" for configuration "Release"
set_property(TARGET wx::wxrichtext APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxrichtext PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_richtext.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "wx::wxhtml;wx::wxxml"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_richtext_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxrichtext )
list(APPEND _cmake_import_check_files_for_wx::wxrichtext "${_IMPORT_PREFIX}/lib//wxmsw32u_richtext.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_richtext_vc_x64_custom.dll" )

# Import target "wx::wxstc" for configuration "Release"
set_property(TARGET wx::wxstc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxstc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_stc.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_stc_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxstc )
list(APPEND _cmake_import_check_files_for_wx::wxstc "${_IMPORT_PREFIX}/lib//wxmsw32u_stc.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_stc_vc_x64_custom.dll" )

# Import target "wx::wxxrc" for configuration "Release"
set_property(TARGET wx::wxxrc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxxrc PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_xrc.lib"
  IMPORTED_LINK_DEPENDENT_LIBRARIES_RELEASE "wx::wxhtml;wx::wxxml"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_xrc_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxxrc )
list(APPEND _cmake_import_check_files_for_wx::wxxrc "${_IMPORT_PREFIX}/lib//wxmsw32u_xrc.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_xrc_vc_x64_custom.dll" )

# Import target "wx::wxgl" for configuration "Release"
set_property(TARGET wx::wxgl APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxgl PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_gl.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_gl_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxgl )
list(APPEND _cmake_import_check_files_for_wx::wxgl "${_IMPORT_PREFIX}/lib//wxmsw32u_gl.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_gl_vc_x64_custom.dll" )

# Import target "wx::wxqa" for configuration "Release"
set_property(TARGET wx::wxqa APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxqa PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxmsw32u_qa.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxmsw32u_qa_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxqa )
list(APPEND _cmake_import_check_files_for_wx::wxqa "${_IMPORT_PREFIX}/lib//wxmsw32u_qa.lib" "${_IMPORT_PREFIX}/bin//wxmsw32u_qa_vc_x64_custom.dll" )

# Import target "wx::wxxml" for configuration "Release"
set_property(TARGET wx::wxxml APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(wx::wxxml PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib//wxbase32u_xml.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin//wxbase32u_xml_vc_x64_custom.dll"
  )

list(APPEND _cmake_import_check_targets wx::wxxml )
list(APPEND _cmake_import_check_files_for_wx::wxxml "${_IMPORT_PREFIX}/lib//wxbase32u_xml.lib" "${_IMPORT_PREFIX}/bin//wxbase32u_xml_vc_x64_custom.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
