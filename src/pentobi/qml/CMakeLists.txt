FILE(GLOB qml_SRC "*.qml" "*.js" "i18n/*.ts" "themes/*/*.qml")
add_custom_target(qml SOURCES ${qml_SRC})
