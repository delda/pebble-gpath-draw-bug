/*
  Demonstrating a drawing bug where a filled path will not display when the layer it is drawn on is past a certain y offset
  */
#include <pebble.h>

static Window *window;
static Layer *path_layer;

static const GPathInfo TEST_PATH = {
  3,
  (GPoint []) {
    {4, 0},
    {0, 4},
    {8, 4}
  }
};
static GPath *test_path_filled;
static GPath *test_path_outline;
int y;

/*
  Click handlers move the layer up and down the screen. Interestingly, the larger the size of the layer, the farther down the layer can go before the filled path disappears, up until the layer reaches 144 pixels.
*/
static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  y = 0;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer_set_frame(path_layer, (GRect) { .origin = { 0, y }, .size = { bounds.size.w, 144 }});
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  y -= 10;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer_set_frame(path_layer, (GRect) { .origin = { 0, y }, .size = { bounds.size.w, 144 }});
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  y += 10;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  layer_set_frame(path_layer, (GRect) { .origin = { 0, y }, .size = { bounds.size.w, 144 }});
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void draw_layer(Layer *layer, GContext *ctx) {
  graphics_context_set_stroke_color(ctx, GColorBlack);
  GRect bounds = layer_get_bounds(layer);
  graphics_fill_rect(ctx,
    bounds,
    0,
    GCornersAll
  );
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_draw_filled(ctx, test_path_filled);
  gpath_draw_outline(ctx, test_path_outline);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  const bool animated = true;
  window_stack_push(window, animated);

  test_path_filled = gpath_create(&TEST_PATH);
  gpath_move_to(test_path_filled, (GPoint) { 72, 2 });
  test_path_outline = gpath_create(&TEST_PATH);
  gpath_move_to(test_path_outline, (GPoint) { 62, 2 });

  y = 0;
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  path_layer = layer_create((GRect) { .origin = { 0, y }, .size = { bounds.size.w, bounds.size.h }});
  layer_set_update_proc(path_layer, draw_layer);
  layer_add_child(window_layer, path_layer);
}

static void deinit(void) {
  gpath_destroy(test_path_filled);
  gpath_destroy(test_path_outline);
  layer_destroy(path_layer);
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
