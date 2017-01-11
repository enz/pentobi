# Some versions of KDECMakeSettings (used in src/pentobi_kde_thumbnailer)
# automatically add a test that globally checks all appstream files in
# the project and fails if they don't use reverse domain IDs (and would
# even fail if they did because of bugs in the test).
set(CTEST_CUSTOM_TESTS_IGNORE appstreamtest)
