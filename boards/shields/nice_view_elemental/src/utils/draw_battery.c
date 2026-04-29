#include "../../include/utils/draw_battery.h"

#include "../../include/colors.h"
#include <lvgl.h>

LV_IMG_DECLARE(battery);
static void draw_battery_outline(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y) {
  lv_draw_image_dsc_t img_dsc;
  lv_draw_image_dsc_init(&img_dsc);
  img_dsc.src = &battery;

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);

  lv_area_t coords = {x, y, x + battery.header.w - 1, y + battery.header.h - 1};

  lv_draw_image(&layer, &img_dsc, &coords);

  lv_canvas_finish_layer(canvas, &layer);
}

LV_IMG_DECLARE(battery_bolt);
static void draw_battery_lightning_bolt(lv_obj_t *canvas, lv_coord_t x,
                                        lv_coord_t y) {
  lv_draw_image_dsc_t img_dsc;
  lv_draw_image_dsc_init(&img_dsc);
  img_dsc.src = &battery_bolt;

  lv_layer_t layer;
  lv_canvas_init_layer(canvas, &layer);

  lv_area_t coords = {x + 10, y + 3, x + 10 + battery_bolt.header.w - 1,
                      y + 3 + battery_bolt.header.h - 1};

  lv_draw_image(&layer, &img_dsc, &coords);

  lv_canvas_finish_layer(canvas, &layer);
}

void draw_battery(lv_obj_t *canvas, lv_coord_t x, lv_coord_t y,
                  struct battery_state state) {
  draw_battery_outline(canvas, x, y);

  // Draw the main part of the battery
  const int width = 19 * (state.level / 100.0);
  if (width > 0) {
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    // fill the empty part
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = BACKGROUND_COLOR;
    lv_area_t coords = {x + 3 + width, y + 3, x + 21, y + 9};
    lv_draw_rect(&layer, &rect_dsc, &coords);

    lv_canvas_finish_layer(canvas, &layer);

    // Round the corners
    lv_canvas_set_px(canvas, x + 3, y + 3, BACKGROUND_COLOR,
                     LV_OPA_COVER); // Top left
    lv_canvas_set_px(canvas, x + 21, y + 3, BACKGROUND_COLOR,
                     LV_OPA_COVER); // Top right
    lv_canvas_set_px(canvas, x + 21, y + 9, BACKGROUND_COLOR,
                     LV_OPA_COVER); // Bottom right
    lv_canvas_set_px(canvas, x + 3, y + 9, BACKGROUND_COLOR,
                     LV_OPA_COVER); // Bottom left
  }

  if (state.is_charging) {
    draw_battery_lightning_bolt(canvas, x, y);
  }
}
