#include "pebble.h"

static Window *s_main_window;
static TextLayer *s_time_layer;
static GBitmap *s_bitmap;
static BitmapLayer *s_bitmap_layer;

static void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  static char s_time_buffer[16];
  if (clock_is_24h_style()) {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%k:%M", tick_time);
  } else {
    strftime(s_time_buffer, sizeof(s_time_buffer), "%l:%M", tick_time);
    if(s_time_buffer[0] == ' '){
       memmove(s_time_buffer, &s_time_buffer[1], sizeof(s_time_buffer) - 1);
    }
  }
  text_layer_set_text(s_time_layer, s_time_buffer);
}

static void main_window_load(Window *window){
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  s_bitmap_layer = bitmap_layer_create(bounds);
  s_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FILM_INV);
  bitmap_layer_set_bitmap(s_bitmap_layer, s_bitmap);
 
#ifdef PBL_PLATFORM_APLITE
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpAssign);
#elif PBL_PLATFORM_BASALT
    bitmap_layer_set_compositing_mode(s_bitmap_layer, GCompOpSet);
#endif
  
  layer_add_child(window_layer, bitmap_layer_get_layer(s_bitmap_layer));
  
  GFont custom_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FUTURA_BOLD_46));
  s_time_layer = text_layer_create(GRect(0, 46, bounds.size.w, 75));
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, custom_font);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  
  time_t now = time(NULL);
  struct tm *current_time = localtime(&now);
  handle_minute_tick(current_time, MINUTE_UNIT);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void main_window_unload(Window *window){
  tick_timer_service_unsubscribe();
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_bitmap_layer);
  gbitmap_destroy(s_bitmap);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorWhite);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
