option(ANAKIN_TEST_KAIST "Use KAIST tests" OFF)
if (ANAKIN_TEST_KAIST)
  add_subdirectory(kaist)
endif()
