#include "../../include/utils/draw_bluetooth_logo_outlined.h"

#include <lvgl.h>

LV_IMG_DECLARE(bluetooth_logo_outlined);

void draw_bluetooth_logo_outlined(lv_obj_t *canvas, lv_coord_t x,
                                  lv_coord_t y) {
  lv_draw_image_dsc_t img_dsc;
  lv_draw_image_dsc_init(&img_dsc);
  img_dsc.src = &bluetooth_logo_outlined;

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);

  lv_area_t coords = {x, y, x + bluetooth_logo_outlined.header.w - 1,
                      y + bluetooth_logo_outlined.header.h - 1};

  lv_draw_image(&layer, &img_dsc, &coords);

  lv_canvas_finish_layer(canvas, &layer);
}
