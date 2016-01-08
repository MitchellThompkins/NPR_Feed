#include "pebble.h"

static Window *s_main_window;
static BitmapLayer *s_image_layer;
static GBitmap *s_image_bitmap;

// TMT here
static void in_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *symbol_tuple = dict_find(iter, QuoteKeySymbol);
  Tuple *price_tuple = dict_find(iter, QuoteKeyPrice);

  if (symbol_tuple) {
    strncpy(s_symbol, symbol_tuple->value->cstring, 5);
    text_layer_set_text(s_symbol_layer, s_symbol);
  }
  if (price_tuple) {
    strncpy(s_price, price_tuple->value->cstring, 10);
    text_layer_set_text(s_price_layer, s_price);
  }
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOADING);

  s_image_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_image_layer, s_image_bitmap);
  bitmap_layer_set_alignment(s_image_layer, GAlignCenter);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_image_layer));
}

static void main_window_unload(Window *window) {
  bitmap_layer_destroy(s_image_layer);
  gbitmap_destroy(s_image_bitmap);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  app_message_register_inbox_received(in_received_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
