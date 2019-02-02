# We don't want to run appstreamtest added by KDECMakeSettings because it
# requires an Internet connection to check the screenshot images. This file
# must be copied to the build root directory to disable the test.
set(CTEST_CUSTOM_TESTS_IGNORE appstreamtest)
