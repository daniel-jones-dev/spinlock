# MIT License
#
# Copyright (c) 2019 Daniel Jones
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

cmake_minimum_required(VERSION 3.10)
project(spinlock)

set(CMAKE_CXX_STANDARD 14)

add_library(spinlock INTERFACE)
target_include_directories(spinlock INTERFACE include)
target_sources(spinlock
  INTERFACE
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include/spinlock/spinlock.h>
    $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include/spinlock/shared_spinlock.h>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/spinlock/spinlock.h>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/spinlock/shared_spinlock.h>
)

add_subdirectory(tests)
