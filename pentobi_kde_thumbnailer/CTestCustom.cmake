# We don't want to run appstreamtest added by KDECMakeSettings if
# PENTOBI_BUILD_KDE_THUMBNAILER because it requires an Internet connection
# to check the screenshot images.
set(CTEST_CUSTOM_TESTS_IGNORE appstreamtest)
