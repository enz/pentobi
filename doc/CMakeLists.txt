if(PENTOBI_BUILD_GUI)
    install(DIRECTORY help DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
        FILES_MATCHING PATTERN "*.css" PATTERN "*.html" PATTERN "*.png"
        PATTERN "*.jpg")
endif()

add_subdirectory(man)
