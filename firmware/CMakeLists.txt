cmake_minimum_required(VERSION 3.16)

# EXTRA_COMPONENT_DIRS must be set to include the "src" directory before
# including "project.cmake."  Otherwise, the build will fail and report:
# "undefined reference to `app_main'"
set(EXTRA_COMPONENT_DIRS "src")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)
