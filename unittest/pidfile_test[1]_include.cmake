if(EXISTS "/home/warrior/project/miniob/unittest/pidfile_test[1]_tests.cmake")
  include("/home/warrior/project/miniob/unittest/pidfile_test[1]_tests.cmake")
else()
  add_test(pidfile_test_NOT_BUILT pidfile_test_NOT_BUILT)
endif()