#include "../../include/central/render.h"

#include "../../include/central/initialize_listeners.h"
#include "../../include/colors.h"
#include "../../include/fonts/press_start_2p_16.h"
#include "../../include/fonts/press_start_2p_8.h"
#include "../../include/main.h"
#include "../../include/utils/draw_battery.h"
#include "../../include/utils/draw_bluetooth_logo.h"
#include "../../include/utils/draw_bluetooth_logo_outlined.h"
#include "../../include/utils/draw_bluetooth_searching.h"
#include <ctype.h>
#include <lvgl.h>
#include <string.h>
#include <zephyr/sys/util.h>

void draw_label_with_outline(lv_obj_t *canvas, lv_layer_t *layer,
                             lv_draw_label_dsc_t *dsc, lv_area_t *origin,
                             lv_color_t outline_color) {
  lv_color_t initial_color = dsc->color;
  dsc->color = outline_color;

  { // Top left
    lv_area_t coords = {origin->x1 - 1, origin->y1 - 1, origin->x2 - 1,
                        origin->y2 - 1};
    lv_draw_label(layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, layer);
  }
  { // Top right
    lv_area_t coords = {origin->x1 + 1, origin->y1 - 1, origin->x2 + 1,
                        origin->y2 - 1};
    lv_draw_label(layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, layer);
  }
  { // Bottom left
    lv_area_t coords = {origin->x1 - 1, origin->y1 + 1, origin->x2 - 1,
                        origin->y2 + 1};
    lv_draw_label(layer, dsc, &coords);
    lv_canvas_finish_layer(canvas, layer);
  }
  {
    lv_area_t coords = {origin->x1 + 1, origin->y1 + 1, origin->x2 + 1,
                        origin->y2 + 1};
    lv_draw_label(layer, dsc, &coords);
  }

  dsc->color = initial_color;
  lv_draw_label(layer, dsc, origin);
  lv_canvas_finish_layer(canvas, layer);
}

void render_battery() {
  lv_canvas_fill_bg(battery_canvas, BACKGROUND_COLOR, LV_OPA_TRANSP);

  draw_battery(battery_canvas, 0, 0, states.battery);
}

static void render_bluetooth_logo() {
  if (states.connectivity.active_profile_bonded) {
    if (states.connectivity.active_profile_connected) {
      draw_bluetooth_logo(connectivity_canvas, 16, 0);
    } else {
      draw_bluetooth_logo_outlined(connectivity_canvas, 16, 0);
    }
  } else {
    draw_bluetooth_searching(connectivity_canvas, 16, 0);
  }
}

static void render_bluetooth_profile_index() {
  lv_draw_label_dsc_t label_dsc;
  lv_draw_label_dsc_init(&label_dsc);
  label_dsc.color = FOREGROUND_COLOR;
  label_dsc.font = &press_start_2p_8;
  label_dsc.align = LV_TEXT_ALIGN_RIGHT;

  // Magic number
  static const unsigned padding_x_offset = 2;
  static const unsigned padding_x = padding_x_offset;

  // Magic number
  static const unsigned padding_y_offset = 1;
  const unsigned padding_y =
      (CONNECTIVITY_CANVAS_HEIGHT - press_start_2p_8.line_height) / 2 +
      padding_y_offset;
  static const unsigned width = CONNECTIVITY_CANVAS_WIDTH - 12 - 2;

  const int idx = states.connectivity.active_profile_index;
  const char *label;
  if (idx >= 0 && idx <= 8) {
    static char single_digit[2];
    single_digit[0] = '1' + idx;
    single_digit[1] = '\0';
    label = single_digit;
  } else {
    label = "?";
  }
  label_dsc.text = label;

  lv_layer_t layer;
  lv_canvas_init_layer(connectivity_canvas, &layer);

  lv_area_t coords_rect = {padding_x + 1, padding_y + 1,
                           padding_x + width - 1 - 3,
                           padding_y + press_start_2p_16.line_height - 1 - 1};

  draw_label_with_outline(connectivity_canvas, &layer, &label_dsc, &coords_rect,
                          BACKGROUND_COLOR);

  lv_canvas_finish_layer(connectivity_canvas, &layer);
}

static void render_bluetooth_connectivity() {
  render_bluetooth_logo();
  render_bluetooth_profile_index();
}

LV_IMG_DECLARE(usb);
static void render_usb_connectivity() {
  lv_draw_image_dsc_t img_dsc;
  lv_draw_image_dsc_init(&img_dsc);
  img_dsc.src = &usb;

  lv_layer_t layer;
  lv_canvas_init_layer(connectivity_canvas, &layer);

  lv_area_t coords = {7, 4, 7 + usb.header.w - 1, 4 + usb.header.h - 1};

  lv_draw_image(&layer, &img_dsc, &coords);

  lv_canvas_finish_layer(connectivity_canvas, &layer);
}

void render_connectivity() {
  lv_canvas_fill_bg(connectivity_canvas, BACKGROUND_COLOR, LV_OPA_TRANSP);

  switch (states.connectivity.selected_endpoint.transport) {
  case ZMK_TRANSPORT_BLE: {
    render_bluetooth_connectivity();
    break;
  }
  case ZMK_TRANSPORT_USB: {
    render_usb_connectivity();
    break;
  }
  case ZMK_TRANSPORT_NONE: {
    render_bluetooth_connectivity();
    break;
  }
  }
}

#define LAYER_TEXT_BUF_SIZE 32

void render_main() {
  lv_layer_t layer;
  lv_canvas_init_layer(layer_canvas, &layer);

  lv_canvas_fill_bg(layer_canvas, BACKGROUND_COLOR, LV_OPA_TRANSP);

  // Capitalize the layer name if given or use the layer number otherwise.
  char text[LAYER_TEXT_BUF_SIZE];
  if (states.layer.name == NULL) {
    snprintf(text, LAYER_TEXT_BUF_SIZE, "LAYER %i", states.layer.index);
  } else {
    size_t len = strlen(states.layer.name);
    if (len >= LAYER_TEXT_BUF_SIZE) {
      // Truncate with ellipsis
      memcpy(text, states.layer.name, LAYER_TEXT_BUF_SIZE - 4);
      strcpy(text + LAYER_TEXT_BUF_SIZE - 4, "...");
    } else {
#if IS_ENABLED(NICE_VIEW_ELEMENTAL_CAPITALIZATION)
      for (size_t i = 0; states.layer.name[i] != '\0'; i++) {
        text[i] = toupper(states.layer.name[i]);
      }
#else
      strcpy(text, states.layer.name);
#endif
      text[len] = '\0';
    }
  }

  lv_draw_label_dsc_t layer_name_dsc;
  lv_draw_label_dsc_init(&layer_name_dsc);
  layer_name_dsc.color = FOREGROUND_COLOR;
  layer_name_dsc.font = &press_start_2p_16;
  layer_name_dsc.align = LV_TEXT_ALIGN_CENTER;
  layer_name_dsc.text = text;

  lv_area_t layer_name_coords_rect = {1, 1, LAYER_CANVAS_WIDTH - 1 - 1,
                                      LAYER_CANVAS_HEIGHT - 1 - 1};

  draw_label_with_outline(layer_canvas, &layer, &layer_name_dsc,
                          &layer_name_coords_rect, BACKGROUND_COLOR);

  lv_canvas_finish_layer(layer_canvas, &layer);
}

void render_modifiers() {
  lv_canvas_fill_bg(modifiers_canvas, BACKGROUND_COLOR, LV_OPA_TRANSP);

  lv_layer_t layer;
  lv_canvas_init_layer(modifiers_canvas, &layer);

  lv_draw_rect_dsc_t inactive_modifier_background_dsc;
  lv_draw_rect_dsc_init(&inactive_modifier_background_dsc);
  inactive_modifier_background_dsc.bg_color = BACKGROUND_COLOR;
  inactive_modifier_background_dsc.border_color = BACKGROUND_COLOR;
  inactive_modifier_background_dsc.border_width = 1;
  inactive_modifier_background_dsc.border_side = LV_BORDER_SIDE_FULL;

  lv_draw_rect_dsc_t active_modifier_background_dsc;
  lv_draw_rect_dsc_init(&active_modifier_background_dsc);
  active_modifier_background_dsc.bg_color = FOREGROUND_COLOR;
  active_modifier_background_dsc.border_color = BACKGROUND_COLOR;
  active_modifier_background_dsc.border_width = 1;
  active_modifier_background_dsc.border_side = LV_BORDER_SIDE_FULL;

  lv_draw_label_dsc_t active_modifier_text_dsc;
  lv_draw_label_dsc_init(&active_modifier_text_dsc);
  active_modifier_text_dsc.color = BACKGROUND_COLOR;
  active_modifier_text_dsc.font = &press_start_2p_8;
  active_modifier_text_dsc.align = LV_TEXT_ALIGN_LEFT;

  lv_draw_label_dsc_t inactive_modifier_text_dsc;
  lv_draw_label_dsc_init(&inactive_modifier_text_dsc);
  inactive_modifier_text_dsc.color = FOREGROUND_COLOR;
  inactive_modifier_text_dsc.font = &press_start_2p_8;
  inactive_modifier_text_dsc.align = LV_TEXT_ALIGN_LEFT;

  {
    {
      lv_area_t coords = {0, // 0 * (MODIFIER_WIDTH + MODIFIER_PADDING_X),
                          0, 0 + MODIFIER_WIDTH - 1,
                          0 + MODIFIERS_CANVAS_HEIGHT - 1};
      lv_draw_rect(&layer,
                   states.modifiers.is_gui_active
                       ? &active_modifier_background_dsc
                       : &inactive_modifier_background_dsc,
                   &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
    {
      lv_area_t coords = {
          BORDER_WIDTH + MODIFIER_PADDING_X, BORDER_WIDTH + MODIFIER_PADDING_Y,
          BORDER_WIDTH + MODIFIER_PADDING_X + MODIFIER_WIDTH - 1,
          BORDER_WIDTH + MODIFIER_PADDING_Y + press_start_2p_8.line_height - 1};
      active_modifier_text_dsc.text = "M";
      inactive_modifier_text_dsc.text = "M";
      lv_draw_label(&layer,
                    states.modifiers.is_gui_active
                        ? &active_modifier_text_dsc
                        : &inactive_modifier_text_dsc,
                    &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
  }

  {
    {
      lv_area_t coords = {1 * (MODIFIER_WIDTH + MODIFIERS_GAP), 0,
                          1 * (MODIFIER_WIDTH + MODIFIERS_GAP) +
                              MODIFIER_WIDTH - 1,
                          0 + MODIFIERS_CANVAS_HEIGHT - 1};
      lv_draw_rect(&layer,
                   states.modifiers.is_alt_active
                       ? &active_modifier_background_dsc
                       : &inactive_modifier_background_dsc,
                   &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
    {
      lv_area_t coords = {1 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X,
                          BORDER_WIDTH + MODIFIER_PADDING_Y,
                          1 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X + MODIFIER_WIDTH - 1,
                          BORDER_WIDTH + MODIFIER_PADDING_Y +
                              press_start_2p_8.line_height - 1};
      active_modifier_text_dsc.text = "A";
      inactive_modifier_text_dsc.text = "A";
      lv_draw_label(&layer,
                    states.modifiers.is_alt_active
                        ? &active_modifier_text_dsc
                        : &inactive_modifier_text_dsc,
                    &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
  }

  {
    {
      lv_area_t coords = {2 * (MODIFIER_WIDTH + MODIFIERS_GAP), 0,
                          2 * (MODIFIER_WIDTH + MODIFIERS_GAP) +
                              MODIFIER_WIDTH - 1,
                          0 + MODIFIERS_CANVAS_HEIGHT - 1};
      lv_draw_rect(&layer,
                   states.modifiers.is_ctrl_active
                       ? &active_modifier_background_dsc
                       : &inactive_modifier_background_dsc,
                   &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
    {
      lv_area_t coords = {2 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X,
                          BORDER_WIDTH + MODIFIER_PADDING_Y,
                          2 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X + MODIFIER_WIDTH - 1,
                          BORDER_WIDTH + MODIFIER_PADDING_Y +
                              press_start_2p_8.line_height - 1};
      active_modifier_text_dsc.text = "C";
      inactive_modifier_text_dsc.text = "C";
      lv_draw_label(&layer,
                    states.modifiers.is_ctrl_active
                        ? &active_modifier_text_dsc
                        : &inactive_modifier_text_dsc,
                    &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
  }

  {
    {
      lv_area_t coords = {3 * (MODIFIER_WIDTH + MODIFIERS_GAP), 0,
                          3 * (MODIFIER_WIDTH + MODIFIERS_GAP) +
                              MODIFIER_WIDTH - 1,
                          0 + MODIFIERS_CANVAS_HEIGHT - 1};
      lv_draw_rect(&layer,
                   states.modifiers.is_shift_active
                       ? &active_modifier_background_dsc
                       : &inactive_modifier_background_dsc,
                   &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
    {
      lv_area_t coords = {3 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X,
                          BORDER_WIDTH + MODIFIER_PADDING_Y,
                          3 * (MODIFIER_WIDTH + MODIFIERS_GAP) + BORDER_WIDTH +
                              MODIFIER_PADDING_X + MODIFIER_WIDTH - 1,
                          BORDER_WIDTH + MODIFIER_PADDING_Y +
                              press_start_2p_8.line_height - 1};
      active_modifier_text_dsc.text = "S";
      inactive_modifier_text_dsc.text = "S";
      lv_draw_label(&layer,
                    states.modifiers.is_shift_active
                        ? &active_modifier_text_dsc
                        : &inactive_modifier_text_dsc,
                    &coords);
      lv_canvas_finish_layer(modifiers_canvas, &layer);
    }
  }
}

// Scrolling background
LV_IMG_DECLARE(stripe_0);
#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
LV_IMG_DECLARE(stripe_double);

static const int32_t stripe_tile_height = 68;

static lv_anim_t anim_template;
static lv_anim_t *running_anim = NULL;
#endif

void initialize_animation() {
#if IS_ENABLED(CONFIG_NICE_VIEW_ELEMENTAL_ANIMATION)
  lv_image_set_src(image_canvas, &stripe_double);
  lv_anim_init(&anim_template);
  lv_anim_set_var(&anim_template, image_canvas);
  lv_anim_set_exec_cb(&anim_template,
                      (lv_anim_exec_xcb_t)lv_image_set_offset_y);
  lv_anim_set_values(&anim_template, 0, -stripe_tile_height);
  lv_anim_set_duration(&anim_template,
                       stripe_tile_height * ANIM_DURATION_CENTRAL / 4);
#else
  lv_image_set_src(image_canvas, &stripe_0);
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
