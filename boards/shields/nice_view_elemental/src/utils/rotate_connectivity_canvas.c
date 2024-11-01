#include "../../include/utils/rotate_connectivity_canvas.h"

#include <lvgl.h>
#include "../../include/main.h"
#include "../../include/colors.h"

void rotate_connectivity_canvas() {
    static lv_color_t cbuf_tmp[LV_CANVAS_BUF_SIZE_TRUE_COLOR(CONNECTIVITY_CANVAS_WIDTH, CONNECTIVITY_CANVAS_HEIGHT)];
    memcpy(cbuf_tmp, connectivity_canvas_buffer, sizeof(cbuf_tmp));

    lv_img_dsc_t img;
    img.data = (void*)cbuf_tmp;
    img.header.cf = LV_IMG_CF_TRUE_COLOR;
    img.header.w = CONNECTIVITY_CANVAS_WIDTH;
    img.header.h = CONNECTIVITY_CANVAS_HEIGHT;

    lv_canvas_fill_bg(connectivity_canvas, BACKGROUND_COLOR, LV_OPA_COVER);
    lv_canvas_transform(
        connectivity_canvas,
        &img,
        900, LV_IMG_ZOOM_NONE,
        -1, 0,
        CONNECTIVITY_CANVAS_WIDTH / 2, CONNECTIVITY_CANVAS_HEIGHT / 2,
        false
    );
}
