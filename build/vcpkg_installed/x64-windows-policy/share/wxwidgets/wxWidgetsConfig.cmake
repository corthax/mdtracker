
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was wxWidgetsConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

include(CMakeFindDependencyMacro)
find_dependency(NanoSVG CONFIG)
if("OFF")
    find_dependency(SDL2 CONFIG)
endif()

cmake_policy(PUSH)
# Set policies to prevent warnings
if(POLICY CMP0072)
    # FindOpenGL prefers GLVND by default when available.
    cmake_policy(GET CMP0072 _OpenGL_GL_POLICY)
    if (_OpenGL_GL_POLICY STREQUAL "")
        cmake_policy(SET CMP0072 NEW)
    endif()
endif()

# determine target from compiler, platform and library type
if(WIN32 AND NOT CYGWIN AND NOT MSYS)
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        set(wxCOMPILER_PREFIX "vc")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        set(wxCOMPILER_PREFIX "gcc")
    elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
        set(wxCOMPILER_PREFIX "clang")
    endif()

    set(wxPLATFORM_ARCH)
    if(CMAKE_GENERATOR_PLATFORM)
        if(NOT CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
            string(TOLOWER ${CMAKE_GENERATOR_PLATFORM} wxPLATFORM_ARCH)
        endif()
    elseif(CMAKE_VS_PLATFORM_NAME)
        if(NOT CMAKE_VS_PLATFORM_NAME STREQUAL "Win32")
            string(TOLOWER ${CMAKE_VS_PLATFORM_NAME} wxPLATFORM_ARCH)
        endif()
    else()
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(wxPLATFORM_ARCH "x64")
        endif()
    endif()

    if(wxPLATFORM_ARCH)
        set(wxARCH_SUFFIX "_${wxPLATFORM_ARCH}")
    endif()

    # use wxWidgets_USE_STATIC to force static libraries, otherwise shared is searched first
    if(NOT wxWidgets_USE_STATIC AND EXISTS "${CMAKE_CURRENT_LIST_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_dll/wxWidgetsTargets.cmake")
        set(wxPLATFORM_LIB_DIR "/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_dll")
    elseif(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_lib/wxWidgetsTargets.cmake")
        set(wxPLATFORM_LIB_DIR "/${wxCOMPILER_PREFIX}${wxARCH_SUFFIX}_lib")
    endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}${wxPLATFORM_LIB_DIR}/wxWidgetsTargets.cmake")

macro(wx_inherit_property source dest name)
    # property name without _<CONFIG>
    get_target_property(prop ${source} ${name})
    if(prop)
        set_target_properties(${dest} PROPERTIES ${name} "${prop}")
    endif()
    # property name with _<CONFIG>
    get_target_property(configs ${source} IMPORTED_CONFIGURATIONS)
    foreach(cfg ${configs})
        set(name_cfg "${name}_${cfg}")
        get_target_property(prop ${source} ${name_cfg})
        if(prop)
            set_target_properties(${dest} PROPERTIES ${name_cfg} "${prop}")
        endif()
    endforeach()
endmacro()

# for compatibility with FindwxWidgets
set(wxWidgets_LIBRARIES)

# create one target with all libraries, same as FindwxWidgets
set(CREATE_WX_TARGET OFF)
if(NOT TARGET wxWidgets::wxWidgets)
    add_library(wxWidgets::wxWidgets INTERFACE IMPORTED)
    set(CREATE_WX_TARGET ON)
endif()

# list all available components
set(wxWidgets_COMPONENTS)
foreach(libname wxbase;wxnet;wxcore;wxadv;wxaui;wxhtml;wxpropgrid;wxribbon;wxrichtext;wxstc;wxxrc;wxgl;wxqa;wxxml)
    if(TARGET wx::${libname})
        # remove wx prefix from component name
        string(SUBSTRING ${libname} 2 -1 name)

        # set variables used in check_required_components
        list(APPEND wxWidgets_COMPONENTS ${name})
        set(wxWidgets_${name}_FOUND 1)
        set(wxWidgets_FIND_REQUIRED_${name} 1)

        # use the Release configuration for MinSizeRel and RelWithDebInfo configurations
        # only when Release target exists, and MinSizeRel/RelWithDebInfo doesn't exist
        get_target_property(configs wx::${libname} IMPORTED_CONFIGURATIONS)
        list(FIND configs "RELEASE" idxSrc)
        if(idxSrc GREATER -1)
            list(FIND configs "MINSIZEREL" idxSrc)
            list(FIND CMAKE_CONFIGURATION_TYPES "MinSizeRel" idxDst)
            if(idxSrc EQUAL -1 AND (idxDst GREATER -1 OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel"))
                set_target_properties(wx::${libname} PROPERTIES MAP_IMPORTED_CONFIG_MINSIZEREL "Release")
            endif()
            list(FIND configs "RELWITHDEBINFO" idxSrc)
            list(FIND CMAKE_CONFIGURATION_TYPES "RelWithDebInfo" idxDst)
            if(idxSrc EQUAL -1 AND (idxDst GREATER -1 OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo"))
                set_target_properties(wx::${libname} PROPERTIES MAP_IMPORTED_CONFIG_RELWITHDEBINFO "Release")
            endif()
        endif()

        # add an alias from wx::<lib> to wx::wx<lib>
        if(CMAKE_VERSION VERSION_LESS "3.18")
            # CMake <3.18 does not support alias to non-global imported target, create a copy of the library
            get_target_property(target_type wx::${libname} TYPE)
            if(target_type STREQUAL STATIC_LIBRARY)
                add_library(wx::${name} STATIC IMPORTED)
            else()
                add_library(wx::${name} SHARED IMPORTED)
            endif()
            wx_inherit_property(wx::${libname} wx::${name} IMPORTED_CONFIGURATIONS)
            wx_inherit_property(wx::${libname} wx::${name} INTERFACE_COMPILE_DEFINITIONS)
            wx_inherit_property(wx::${libname} wx::${name} INTERFACE_INCLUDE_DIRECTORIES)
            wx_inherit_property(wx::${libname} wx::${name} INTERFACE_LINK_LIBRARIES)
            wx_inherit_property(wx::${libname} wx::${name} IMPORTED_LINK_INTERFACE_LANGUAGES)
            wx_inherit_property(wx::${libname} wx::${name} IMPORTED_LOCATION)
            wx_inherit_property(wx::${libname} wx::${name} IMPORTED_IMPLIB)
            wx_inherit_property(wx::${libname} wx::${name} IMPORTED_LINK_DEPENDENT_LIBRARIES)
        else()
            add_library(wx::${name} ALIAS wx::${libname})
        endif()

        # add to FindwxWidgets variable
        list(FIND wxWidgets_FIND_COMPONENTS ${name} idx)
        if(NOT wxWidgets_FIND_COMPONENTS OR idx GREATER -1)
            list(APPEND wxWidgets_LIBRARIES wx::${name})
            if(CREATE_WX_TARGET)
                target_link_libraries(wxWidgets::wxWidgets INTERFACE wx::${name})
            endif()
        endif()
    endif()
endforeach()

if(TARGET wx::wxgl)
    # make sure OpenGL targets are available:
    # The link interface of target "wx::wxgl" contains: OpenGL::GLU
    find_package(OpenGL QUIET)
endif()

# if no components are specified in find_package, check all of them
if(NOT wxWidgets_FIND_COMPONENTS)
    set(wxWidgets_FIND_COMPONENTS ${wxWidgets_COMPONENTS})
endif()

check_required_components("wxWidgets")

cmake_policy(POP)
