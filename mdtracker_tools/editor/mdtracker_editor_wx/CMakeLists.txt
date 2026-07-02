cmake_minimum_required(VERSION 3.20)
project(mdtracker_editor_wx VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(wavpack CONFIG REQUIRED)

include(${wxWidgets_USE_FILE})

add_executable(mdtracker_editor_wx
    main.cpp
    main_frame.cpp
    sample_bank_panel.cpp
    instrument_editor_panel.cpp
    tfi_parser.cpp
    vgi_parser.cpp
    sample_converter_panel.cpp
    save_converter_panel.cpp
    settings_panel.cpp
    settings_service.cpp
    rom_service.cpp
    save_converter_service.cpp
    sample_converter_service.cpp
    dr_flac.h
    resource.rc
)

target_include_directories(mdtracker_editor_wx PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(mdtracker_editor_wx PRIVATE ${wxWidgets_LIBRARIES} nlohmann_json::nlohmann_json WavPack::WavPack)
set_target_properties(mdtracker_editor_wx PROPERTIES WIN32_EXECUTABLE TRUE)
target_compile_definitions(mdtracker_editor_wx PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
