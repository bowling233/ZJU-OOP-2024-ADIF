cmake_minimum_required (VERSION 3.11)

# --- Fetch FTXUI --------------------------------------------------------------
include(FetchContent)

set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG main
)

FetchContent_GetProperties(ftxui)
if(NOT ftxui_POPULATED)
  FetchContent_Populate(ftxui)
  add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# --- Fetch Rapidcsv -----------------------------------------------------------

FetchContent_Declare(rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv
  GIT_TAG main
)

FetchContent_GetProperties(rapidcsv)
if(NOT rapidcsv_POPULATED)
  FetchContent_Populate(rapidcsv)
  add_subdirectory(${rapidcsv_SOURCE_DIR} ${rapidcsv_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ------------------------------------------------------------------------------

project(adif
  LANGUAGES CXX
  VERSION 1.0.0
)

add_library(adif src/adif.cpp)
target_include_directories(adif PUBLIC include)
target_link_libraries(adif PRIVATE rapidcsv)

add_executable(cli src/cli.cpp)
add_executable(tui src/tui.cpp)

target_link_libraries(cli
  PRIVATE rapidcsv
  PRIVATE adif
)

target_link_libraries(tui
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
  PRIVATE rapidcsv
  PRIVATE adif
)
