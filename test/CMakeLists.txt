option(ANAKIN_TEST_KAIST "Use KAIST tests" OFF)
option(ANAKIN_TEST_API "Use api tests" OFF)
if (ANAKIN_TEST_KAIST)
  add_subdirectory(kaist)
endif()

if (ANAKIN_TEST_API)
  add_subdirectory(api)
endif()
