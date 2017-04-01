# Some versions of KDECMakeSettings (used in src/pentobi_kde_thumbnailer)
# automatically add a test that globally checks all appstream files in
# the project and fails if the build directory does not contain
# install_manifest.txt (which exists only after installation).
set(CTEST_CUSTOM_TESTS_IGNORE appstreamtest)
