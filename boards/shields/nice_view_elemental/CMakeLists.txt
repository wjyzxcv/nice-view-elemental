if(CONFIG_ZMK_DISPLAY AND CONFIG_NICE_VIEW_WIDGET_STATUS)
  zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include)

  zephyr_library_sources(
    ./src/main.c
    ./src/images/bluetooth_logo_outlined.c
    ./src/images/bluetooth_logo.c
    ./src/images/bluetooth_searching.c
    ./src/utils/draw_battery.c
    ./src/utils/draw_bluetooth_logo_outlined.c
    ./src/utils/draw_bluetooth_logo.c
    ./src/images/battery.c
    ./src/images/battery_bolt.c
    ./src/images/usb.c
  )

  if(NOT CONFIG_ZMK_SPLIT OR CONFIG_ZMK_SPLIT_ROLE_CENTRAL)
    zephyr_library_sources(
      ./src/utils/draw_bluetooth_searching.c
      ./src/central/initialize_listeners.c
      ./src/central/render.c
      ./src/fonts/press_start_2p_16.c
      ./src/fonts/press_start_2p_8.c
    )
    if(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
      zephyr_library_sources(./src/images/stripe_double.c)
    else()
      zephyr_library_sources(./src/images/stripe_0.c)
    endif()
  else()
    zephyr_library_sources(
      ./src/peripheral/initialize_listeners.c
      ./src/peripheral/render.c
    )
    if(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
      zephyr_library_sources(
        ./src/images/fluffy_boi_0.c
        ./src/images/fluffy_boi_1.c
        ./src/images/fluffy_boi_2.c
        ./src/images/fluffy_boi_3.c
      )
    else()
      zephyr_library_sources(./src/images/fluffy_boi_0.c)
    endif()
  endif()
endif()
