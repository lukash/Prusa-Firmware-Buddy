target_sources(
  firmware
  PRIVATE footer_icon.cpp
          footer_item_axis.cpp
          footer_item_fans.cpp
          footer_item_filament.cpp
          footer_item_fsensor.cpp
          footer_item_live_z.cpp
          footer_item_printspeed.cpp
          footer_item_sheet_profile.cpp
          footer_items_heaters.cpp
          footer_items_nozzle_bed.cpp
          footer_line.cpp
          footer_text.cpp
          ifooter.cpp
          ifooter_item.cpp
          ifooter_string_item.cpp
  )

if(BOARD IN_LIST BOARDS_WITH_ST7789V)
  target_sources(firmware PRIVATE footer_doubleline.cpp)
endif()
