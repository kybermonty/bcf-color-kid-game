#include <application.h>

#define COLOR_COUNT 5

bc_button_t button;
bc_led_strip_t led_strip;

uint32_t color_palette[COLOR_COUNT] = {
    0x0000000F,     // White
    0x0F000000,     // Red
    0x0F000F00,     // Purple
    0x00000F00,     // Blue
    0x000F0000      // Green
};

typedef struct color_list_t {
    uint8_t color;
    struct color_list_t *next;
} color_list_t;

color_list_t *color_list_first = NULL;
uint8_t state = 0;

void button_event_handler(bc_button_t *self, bc_button_event_t event, void *param)
{
    (void) self;
    (void) param;
    if (event == BC_BUTTON_EVENT_PRESS)
    {
        state = !state;
    }
}

void application_init(void)
{
    bc_button_init(&button, BC_GPIO_BUTTON, BC_GPIO_PULL_DOWN, false);
    bc_button_set_event_handler(&button, button_event_handler, NULL);

    bc_module_power_init();
    bc_led_strip_init(&led_strip, bc_module_power_get_led_strip_driver(), &bc_module_power_led_strip_buffer_rgbw_144);
}

void application_task(void)
{
    if (state == 0) {
        bc_led_strip_fill(&led_strip, 0x00000000);

        color_list_t *item = malloc(sizeof(color_list_t));
        item->color = rand() % COLOR_COUNT;
        item->next = color_list_first;
        color_list_first = item;

        color_list_t *item_last = NULL;
        for (int i = 0; i < bc_led_strip_get_pixel_count(&led_strip); i++) {
            bc_led_strip_set_pixel(&led_strip, i, color_palette[item->color]);
            item_last = item;
            item = item->next;
            if (!item) {
                break;
            }
        }
        if (item_last) {
            item_last->next = NULL;
        }
        if (item) {
            free(item);
        }
    } else if (color_list_first) {
        bc_led_strip_fill(&led_strip, color_palette[color_list_first->color]);
    }

    bc_led_strip_write(&led_strip);

    bc_scheduler_plan_current_relative(100);
}
