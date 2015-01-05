if (NOT EXISTS ${CMAKE_BINARY_DIR}/kaist.zip)
  file(DOWNLOAD "http://www.iapr-tc11.org/dataset/KAIST_SceneText/(E.S)D-indoor.zip" ${CMAKE_BINARY_DIR}/kaist.zip SHOW_PROGRESS)
endif()

if (NOT EXISTS ${CMAKE_BINARY_DIR}/kaist_data)
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/kaist_data)
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E tar xvf "${CMAKE_BINARY_DIR}/kaist.zip"
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/kaist_data)
endif ()

foreach (angle 0 10)
  add_test(NAME KaistTest::theta${angle} COMMAND
    ${CMAKE_CURRENT_SOURCE_DIR}/run.sh ${CMAKE_BINARY_DIR}/kaist_data clean ${angle}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endforeach()
