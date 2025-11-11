#include <pebble.h>
#include <ctype.h>

static void str_to_upper(char *s) {
  for (; *s; ++s) *s = (char)toupper((unsigned char)*s);
}

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_top_layer;
static TextLayer *s_date_bottom_layer;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), "%I:%M", tick_time);
  // Remove leading zeros
  if (s_buffer[0] == '0') {
    memmove(s_buffer, s_buffer + 1, sizeof(s_buffer) - 1);
  }
  text_layer_set_text(s_time_layer, s_buffer);

  // Write the current date into a buffer
  if (s_date_top_layer && s_date_bottom_layer) {
    static char top_buf[12];
    static char mon_buf[8]; 

    char wday[4], day[3], mon[4];
    strftime(wday, sizeof(wday), "%a", tick_time);
    strftime(day,  sizeof(day),  "%d", tick_time);
    if (day[0] == '0') memmove(day, day + 1, 2);
    strftime(mon,  sizeof(mon),  "%b", tick_time);

    str_to_upper(wday);
    str_to_upper(mon);

    snprintf(top_buf, sizeof(top_buf), "%s %s", wday, day);
    snprintf(mon_buf, sizeof(mon_buf), "%s", mon);
    //strftime(mon_buf, sizeof(mon_buf), "%b", tick_time);

    text_layer_set_text(s_date_top_layer, top_buf);
    text_layer_set_text(s_date_bottom_layer, mon_buf);
  }
}

static void layout_layers(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  const int16_t margin = PBL_IF_ROUND_ELSE(14, 6);

  // Heights
  const int16_t time_h = 45;
  const int16_t long_date_h = 32;
  const int16_t short_date_h = 21;
  const int16_t bottom_y_time = bounds.size.h - margin - time_h;
  const int16_t bottom_y_long_date = bounds.size.h - margin - long_date_h;
  const int16_t bottom_y_short_date = bounds.size.h - margin - short_date_h;

#if defined(PBL_ROUND)
  // ROUND: time centered at bottom, no date
  GRect time_frame = GRect(margin, bottom_y_time, bounds.size.w - 2 * margin, time_h);
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Hide date on round
  if (s_date_top_layer) {
    layer_set_hidden(text_layer_get_layer(s_date_top_layer), true);
  }
#else
  // RECTANGULAR: time bottom-right, date bottom-left
  const int16_t half_w = bounds.size.w / 2;

  // Sizes
  const int16_t top_h = 22;
  const int16_t mon_h = 22;
  const int16_t gap_y = 2;

  // Date (left bottom)
  if (!s_date_top_layer) {
    s_date_top_layer = text_layer_create(
      GRect(margin, bottom_y_long_date, half_w - 2*margin, top_h)
    );
    text_layer_set_background_color(s_date_top_layer, GColorClear);
    text_layer_set_text_color(s_date_top_layer, GColorBlack);
    text_layer_set_text_alignment(s_date_top_layer, GTextAlignmentLeft);
    text_layer_set_font(s_date_top_layer, s_date_font);
    layer_add_child(root, text_layer_get_layer(s_date_top_layer));
  } else {
    layer_set_frame(
      text_layer_get_layer(s_date_top_layer), 
      GRect(margin, bottom_y_long_date, half_w - 2*margin, top_h)
    );
  }

  if (!s_date_bottom_layer) {
    s_date_bottom_layer = text_layer_create(
      GRect(margin, bottom_y_short_date, half_w - 2*margin, mon_h)
    );
    text_layer_set_background_color(s_date_bottom_layer, GColorClear);
    text_layer_set_text_color(s_date_bottom_layer, GColorBlack);
    text_layer_set_text_alignment(s_date_bottom_layer, GTextAlignmentLeft);
    text_layer_set_font(s_date_bottom_layer, s_date_font);
    layer_add_child(root, text_layer_get_layer(s_date_bottom_layer));
  } else {
    layer_set_frame(
      text_layer_get_layer(s_date_bottom_layer), 
      GRect(margin, bottom_y_short_date, half_w - 2*margin, mon_h)
    );
  }

  // Time (right bottom)
  GRect time_frame = GRect(margin, bottom_y_time, bounds.size.w - 2 * margin, time_h);
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentRight);
#endif
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create GBitmap, Bitmaplayer, set the bitmap onto the layer and add to the window
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  s_background_layer = bitmap_layer_create(bounds);
  bitmap_layer_set_alignment(s_background_layer, GAlignTop);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
  layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));

  // Load date and time fonts
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MICRO_42));
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MICRO_21));

  // Create Time Layer, add styles and append to window's root layer
  s_time_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58, 52), bounds.size.w, 50));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_font(s_time_layer, s_time_font);
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Perform initial layout
  layout_layers(window);
}

static void main_window_unload(Window *window) {
  text_layer_destroy(s_time_layer);
  fonts_unload_custom_font(s_time_font);
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_bitmap);
  if (s_date_bottom_layer) { 
    text_layer_destroy(s_date_bottom_layer);  
    s_date_bottom_layer = NULL;
  }
  if (s_date_top_layer) { 
    text_layer_destroy(s_date_top_layer);
    s_date_top_layer = NULL;
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Change the background color
  window_set_background_color(s_main_window, GColorWhite);

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}