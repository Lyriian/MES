add_executable(Src
        Main.c
        )

# pull in common dependencies and additional i2c hardware support
target_link_libraries(Main pico_stdlib hardware_i2c)

# create map/bin/hex file etc.
pico_add_extra_outputs(Main)

# add url via pico_set_program_url
example_auto_set_url(Main)
