#include "../../include/utils/draw_bluetooth_searching.h"

#include <lvgl.h>

LV_IMG_DECLARE(bluetooth_searching);

void draw_bluetooth_searching(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y) {
  lv_draw_image_dsc_t img_dsc;
  lv_draw_image_dsc_init(&img_dsc);
  img_dsc.src = &bluetooth_searching;

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);

  lv_area_t coords = {x, y, x + bluetooth_searching.header.w - 1,
                      y + bluetooth_searching.header.h - 1};

  lv_draw_image(&layer, &img_dsc, &coords);

  lv_canvas_finish_layer(canvas, &layer);
}
