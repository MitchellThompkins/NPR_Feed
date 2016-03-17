#include "pebble.h"
#include "news_list.h"

static Window *s_news_window;
static TextLayer *s_text_layer;

static void news_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  s_text_layer = text_layer_create(bounds);

  // Set some properties
  text_layer_set_text_color(s_text_layer, GColorBlack);
  text_layer_set_background_color(s_text_layer, GColorWhite);
  text_layer_set_overflow_mode(s_text_layer, GTextOverflowModeWordWrap);

  // Add to the Window
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

  int len = strlen(cur_news.s_titles);
  char *s_buffer = malloc(len);

  snprintf(s_buffer, len, "%s", cur_news.s_titles);
  text_layer_set_text(s_text_layer, s_buffer);
}

// Unload loading window
static void news_window_unload(Window *window) {
  text_layer_destroy(s_text_layer);
}

void news_list_init(int story_count, char *str_titles, char *str_teasers) {
  s_news_window = window_create();
  window_set_window_handlers(s_news_window, (WindowHandlers) {
    .load = news_window_load,
    .unload = news_window_unload,
  });
  window_stack_push(s_news_window, true);
}

void news_list_deinit(void){
  window_destroy(s_news_window);
}
