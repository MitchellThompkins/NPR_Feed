#include "pebble.h"
#include "news_list.h"
#include "externs.h"

#define NUM_MENU_SECTIONS 1

static void news_window_init();

static Window *news_list_window;
static Window *news_window;

static MenuLayer *menu_layer;
static TextLayer *news_text_layer;
static ScrollLayer *news_scroll_layer;

static int cur_sel;
static int16_t s_scroll_index;
static int16_t s_scroll_row_index;
static AppTimer *s_scroll_timer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return str_count; // Variable story count
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  char time_buffer[80];
  char update_time_msg[100];
  clock_copy_time_string(time_buffer, sizeof(time_buffer));
  snprintf(update_time_msg, sizeof(update_time_msg), "Last Upadated: %s", time_buffer);
  menu_cell_basic_header_draw(ctx, cell_layer, update_time_msg);
}

static void scroll_timer_callback(void *data)
{
  s_scroll_index+=3;
  if(s_scroll_row_index>0)
  layer_mark_dirty(menu_layer_get_layer(menu_layer));
  s_scroll_timer = app_timer_register(1000,scroll_timer_callback,NULL);
}

static void menu_cell_animated_draw(GContext* ctx, const Layer* cell_layer, char* text, char* subtext, bool animate)
{
  if(animate && s_scroll_index>0)
  {
    if(((int16_t)strlen(text)-15-s_scroll_index)>0)
    text+=s_scroll_index;
  }
  menu_cell_basic_draw(ctx,cell_layer,text,subtext,NULL);
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  bool animate = cell_index->row==s_scroll_row_index;
  int cur_row = cell_index->row;
  menu_cell_basic_draw(ctx, cell_layer, *(str_titles + cur_row), NULL, NULL);

  // menu_cell_animated_draw(ctx, cell_layer, *(str_titles + cur_row), NULL, animate);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  cur_sel = cell_index->row;
  news_window_init();
}

static void menu_selection_changed_callback(MenuLayer *menu_layer, MenuIndex new_index, MenuIndex old_index, void *callback_context) {
  s_scroll_index=0;
  s_scroll_row_index = new_index.row;
  app_timer_reschedule(s_scroll_timer,1000);
}


int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	return 50;
}

static void news_list_window_load(Window *window) {

  for (int i = 0; i < str_count; i++) {
    int len = strlen(*(str_teasers + i)) + 1;
    char *s_buffer = malloc(len);
    snprintf(s_buffer, len, "%s", *(str_teasers + i));
    APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
    free(s_buffer);
  }

  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Create the menu layer
  menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .selection_changed = menu_selection_changed_callback,
    .get_cell_height = menu_get_cell_height_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void news_list_window_unload(Window *window) {
  // Destroy the menu layer
  menu_layer_destroy(menu_layer);
}

void news_list_window_init(Window *loading_window) {

  for (int i = 0; i < str_count; i++) {
    int len = strlen(*(str_teasers + i)) + 1;
    char *s_buffer = malloc(len);
    snprintf(s_buffer, len, "%s", *(str_teasers + i));
    APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
    free(s_buffer);
  }

  news_list_window = window_create();
  window_set_window_handlers(news_list_window, (WindowHandlers) {
    .load = news_list_window_load,
    .unload = news_list_window_unload,
  });
  s_scroll_timer = app_timer_register(500,scroll_timer_callback,NULL);
  window_stack_push(news_list_window, true);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap bytes (free: %d, used: %d)", heap_bytes_free(), heap_bytes_used());
  window_stack_remove(loading_window, true); // TMT
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap bytes (free: %d, used: %d)", heap_bytes_free(), heap_bytes_used());
}

void news_list_window_deinit() {
  window_destroy(news_list_window);
  window_stack_pop_all(true); // TMT
}

static void news_window_load(Window *window) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Here.");
  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  GFont font = fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD);

  // Find the bounds of the scrolling text
  GRect shrinking_rect = GRect(0, 0, bounds.size.w, 2000);
  // int len_title = strlen(*(str_titles + cur_sel));
  // int len_teaser = strlen(*(str_teasers + cur_sel));
  // int len_total = len_title + len_teaser;
  //
  // // // TMT Here
  // // char *s_buffer = calloc(len_total + 3); // 2 for ': ' and 1 for null char
  // // strcat(s_buffer, ": ");
  // // strcat(s_buffer, *);

  for (int i = 0; i < str_count; i++) {
    int len = strlen(*(str_teasers + i)) + 1;
    char *s_buffer = malloc(len);
    snprintf(s_buffer, len, "%s", *(str_teasers + i));
    APP_LOG(APP_LOG_LEVEL_DEBUG, "*(str_teasers + cur_sel)");
    APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);
    free(s_buffer);
  }

  int len = strlen(*(str_teasers + cur_sel)) + 1;
  char *s_buffer = malloc(len);
  snprintf(s_buffer, len, "%s", *(str_teasers + cur_sel));
  strcpy(s_buffer, *(str_teasers + cur_sel));
  APP_LOG(APP_LOG_LEVEL_DEBUG, s_buffer);

  GSize text_size = graphics_text_layout_get_content_size(s_buffer, font,
                  shrinking_rect, GTextOverflowModeWordWrap, GTextAlignmentLeft);
  GRect text_bounds = bounds;
  text_bounds.size.h = text_size.h;

  // Create the TextLayer
  news_text_layer = text_layer_create(text_bounds);
  text_layer_set_overflow_mode(news_text_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(news_text_layer, font);
  text_layer_set_text(news_text_layer, s_buffer);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Heap bytes (free: %d, used: %d)", heap_bytes_free(), heap_bytes_used());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "cur_sel: %d", cur_sel);

  free(s_buffer);

  // Create the ScrollLayer
  news_scroll_layer = scroll_layer_create(bounds);

  // Set the scrolling content size
  scroll_layer_set_content_size(news_scroll_layer, text_size);

  // Let the ScrollLayer receive click events
  scroll_layer_set_click_config_onto_window(news_scroll_layer, window);

  // Add the TextLayer as a child of the ScrollLayer
  scroll_layer_add_child(news_scroll_layer, text_layer_get_layer(news_text_layer));

  // Add the ScrollLayer as a child of the Window
  layer_add_child(window_layer, scroll_layer_get_layer(news_scroll_layer));
}

static void news_window_unload(Window *window) {
  scroll_layer_destroy(news_scroll_layer);
  text_layer_destroy(news_text_layer);
}

static void news_window_init() {
  news_window = window_create();
  window_set_window_handlers(news_window, (WindowHandlers) {
    .load = news_window_load,
    .unload = news_window_unload,
  });
  window_stack_push(news_window, true);
}

static void news_window_deinit() {
  window_destroy(news_window);
}
