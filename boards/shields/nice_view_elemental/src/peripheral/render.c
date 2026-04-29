#include "../../include/peripheral/render.h"

#include "../../include/colors.h"
#include "../../include/main.h"
#include "../../include/peripheral/initialize_listeners.h"
#include "../../include/utils/draw_battery.h"
#include "../../include/utils/draw_bluetooth_logo.h"
#include "../../include/utils/draw_bluetooth_logo_outlined.h"
#include <lvgl.h>
#include <zephyr/sys/util.h>

#include <zephyr/kernel.h>

static lv_obj_t *tmp_battery_canvas = NULL;
static lv_color_t tmp_battery_buffer[LV_CANVAS_BUF_SIZE(
    BATTERY_CANVAS_WIDTH, BATTERY_CANVAS_HEIGHT,
    LV_COLOR_FORMAT_GET_BPP(LV_COLOR_FORMAT_RGB565), LV_DRAW_BUF_STRIDE_ALIGN)];

// lv_draw_sw_rotate currently does not support AL88
void render_battery() {
  if (!tmp_battery_canvas) {
    tmp_battery_canvas = lv_canvas_create(NULL);
    lv_canvas_set_buffer(tmp_battery_canvas, tmp_battery_buffer,
                         BATTERY_CANVAS_WIDTH, BATTERY_CANVAS_HEIGHT,
                         LV_COLOR_FORMAT_RGB565);
  }

  lv_canvas_fill_bg(tmp_battery_canvas, BACKGROUND_COLOR, LV_OPA_COVER);

  draw_battery(tmp_battery_canvas, 0, 0, states.battery);

  const uint32_t src_stride =
      lv_draw_buf_width_to_stride(BATTERY_CANVAS_WIDTH, LV_COLOR_FORMAT_RGB565);
  const uint32_t dst_stride = lv_draw_buf_width_to_stride(
      BATTERY_CANVAS_HEIGHT, LV_COLOR_FORMAT_RGB565);
  lv_draw_sw_rotate(tmp_battery_buffer, battery_canvas_buffer,
                    BATTERY_CANVAS_WIDTH, BATTERY_CANVAS_HEIGHT, src_stride,
                    dst_stride, LV_DISPLAY_ROTATION_270,
                    LV_COLOR_FORMAT_RGB565);
}
void render_connectivity() {
  // lv_canvas_fill_bg(connectivity_canvas, FOREGROUND_COLOR, LV_OPA_TRANSP);

  if (states.connectivity.connected) {
    draw_bluetooth_logo(connectivity_canvas, 16, 0);
  } else {
    draw_bluetooth_logo_outlined(connectivity_canvas, 16, 0);
  }
}

// 1. Insert images here.
LV_IMG_DECLARE(fluffy_boi_0);
#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
LV_IMG_DECLARE(fluffy_boi_1);
LV_IMG_DECLARE(fluffy_boi_2);
LV_IMG_DECLARE(fluffy_boi_3);
static const lv_image_dsc_t *images[] = {
    &fluffy_boi_0,
    &fluffy_boi_1,
    &fluffy_boi_2,
    &fluffy_boi_3,
};

static const unsigned int frame_count = sizeof(images) / sizeof(images[0]);

static lv_anim_t anim_template;
static lv_anim_t *running_anim = NULL;

static void frame_change_cb(lv_obj_t *obj, int32_t idx) {
  if (idx >= frame_count)
    idx = frame_count - 1;
  lv_image_set_src(obj, images[idx]);
}
#endif

void initialize_animation() {
  lv_image_set_src(image_canvas, &fluffy_boi_0);

#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
  lv_anim_init(&anim_template);
  lv_anim_set_var(&anim_template, image_canvas);
  lv_anim_set_exec_cb(&anim_template, (lv_anim_exec_xcb_t)frame_change_cb);
  lv_anim_set_values(&anim_template, 0, frame_count);
  lv_anim_set_duration(&anim_template, frame_count * ANIM_DURATION_PERIPHRAL);
#endif
}

void start_animation() {
#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
  if (running_anim && lv_anim_is_paused(running_anim)) {
    lv_anim_resume(running_anim);
    return;
  }
  if (running_anim)
    return;
  lv_anim_set_repeat_count(&anim_template, LV_ANIM_REPEAT_INFINITE);
  running_anim = lv_anim_start(&anim_template);
#endif
}

void stop_animation() {
#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
  if (running_anim) {
    lv_anim_pause(running_anim);
  }
#endif
}
