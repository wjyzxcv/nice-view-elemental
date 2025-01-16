#include "../../include/utils/draw_fluffy_boi.h"

#include <lvgl.h>
#include "../../include/images/fluffy_boi_0.h"
#include "../../include/images/fluffy_boi_1.h"
#include "../../include/images/fluffy_boi_2.h"
#include "../../include/images/fluffy_boi_3.h"

void draw_fluffy_boi(lv_obj_t *canvas, unsigned index) {
    lv_draw_img_dsc_t img_dsc;
    lv_draw_img_dsc_init(&img_dsc);

    switch (index % 3) {
    case 0: {
        lv_canvas_draw_img(canvas, 0, 0, &fluffy_boi_0, &img_dsc);
        break;
    }
    case 1: {
        lv_canvas_draw_img(canvas, 0, 0, &fluffy_boi_1, &img_dsc);
        break;
    }
    case 2: {
        lv_canvas_draw_img(canvas, 0, 0, &fluffy_boi_2, &img_dsc);
        break;
    }
    case 3: {
        lv_canvas_draw_img(canvas, 0, 0, &fluffy_boi_3, &img_dsc);
        break;
    }
    }
}
