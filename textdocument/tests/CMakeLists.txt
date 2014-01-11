project(grantlee_textdocument_tests)

find_package(Qt5 5.0.0 REQUIRED Test)
include_directories(${QT_INCLUDES})

set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

add_library(textdocument_test_builtins STATIC
  coverageobject.cpp
)
qt5_use_modules(textdocument_test_builtins Core Test)

macro(GRANTLEE_TEXTDOCUMENT_UNIT_TESTS)
  foreach(_testname ${ARGN})
    add_executable(${_testname}_exec ${_testname}.cpp)
    add_test(${_testname} ${_testname}_exec )
    target_link_libraries(${_testname}_exec grantlee_gui textdocument_test_builtins)
    qt5_use_modules(${_testname}_exec Core Test)
    set_property(GLOBAL APPEND PROPERTY TEST_COVERAGE "${CMAKE_CURRENT_BINARY_DIR}/${_testname}_exec" )
  endforeach(_testname)
endmacro(GRANTLEE_TEXTDOCUMENT_UNIT_TESTS)

grantlee_textdocument_unit_tests(
  htmlbuildertest
  plainmarkupbuildertest
)
