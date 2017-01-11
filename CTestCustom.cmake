# Some versions of KDECMakeSettings add a test that globally checks all
# appstream files in the project, insists on reverse domain IDs and fails
# if no install_manifest.txt is in the project's base directory.
set(CTEST_CUSTOM_TESTS_IGNORE appstreamtest)
