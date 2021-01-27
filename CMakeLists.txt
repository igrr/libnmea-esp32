cmake_minimum_required(VERSION 3.12)

set(common "libnmea/src/nmea/nmea.c"
           "libnmea/src/nmea/parser_static.c"
           "libnmea/src/parsers/parse.c"
           )

set(parsers gpgga gpgll gprmc)

idf_component_register(SRCS ${common}
                       INCLUDE_DIRS "libnmea/src/nmea"
                                    "libnmea/src/parsers"
                      )

foreach(parser ${parsers})
    # add source file
    set(src_file "libnmea/src/parsers/${parser}.c")
    target_sources(${COMPONENT_LIB} PRIVATE ${src_file})

    # add some preprocessor definitions to rename the interface functions
    set(prefix nmea_${parser}_)
    set(defs "allocate_data=${prefix}allocate_data"
             "free_data=${prefix}free_data"
             "init=${prefix}init"
             "parse=${prefix}parse"
             "set_default=${prefix}set_default"
    )
    set_source_files_properties(${src_file} PROPERTIES COMPILE_DEFINITIONS "${defs}")

    # Hide incorrect (but harmless) use of strncpy in NMEA_PARSER_PREFIX macro
    set_source_files_properties(${src_file} PROPERTIES COMPILE_OPTIONS "-Wno-stringop-truncation")

    # Enable the parser
    string(TOUPPER ${parser} parser_uppercase)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE "ENABLE_${parser_uppercase}=1")
endforeach()

list(LENGTH parsers parsers_count)
target_compile_definitions(${COMPONENT_LIB} PRIVATE PARSER_COUNT=${parsers_count})
