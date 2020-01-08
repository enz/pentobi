include(FindPackageHandleStandardArgs)
find_path(DOCBOOKXSL_DIR html/chunk.xsl
    HINTS
    /usr/share/xml/docbook/stylesheet/docbook-xsl # Debian
    /usr/local/share/xsl/docbook # FreeBSD
    /usr/share/xml/docbook/stylesheet/nwalsh/current # OpenSUSE
    /usr/share/sgml/docbook/xsl-ns-stylesheets # Fedora
    /usr/share/sgml/docbook/xsl-stylesheets # Fedora
    )
find_package_handle_standard_args(DocBookXSL
    "Could NOT find DocBook XSL stylesheets. Install docbook-xsl and/or use \
    -DDOCBOOKXSL_DIR=<dir> to define its location."
    DOCBOOKXSL_DIR)
mark_as_advanced(DOCBOOKXSL_DIR)
