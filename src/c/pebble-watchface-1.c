#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static GFont s_time_font, s_date_font;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

  // Write the current date into a buffer
  if (s_date_layer) {
    static char s_date_buf[48];
    char wday[4];
    char day[3];
    char mon[8]; 

    /*strftime(s_date_buf, sizeof(s_date_buf), "%d %b", tick_time);
    text_layer_set_text(s_date_layer, s_date_buf);*/

    // Add short weekday, remove possible leading zero day and short month
    strftime(wday, sizeof(wday), "%a", tick_time);
    strftime(day,  sizeof(day),  "%d", tick_time);
    //if (day[0] == '0') memmove(day, day + 1, 2);
    strftime(mon,  sizeof(mon),  "%d %b", tick_time);

    // Short weekday + num day in line 1, Short month in line 2
    snprintf(s_date_buf, sizeof(s_date_buf), "%s %s\n%s", wday, day, mon);
    text_layer_set_text(s_date_layer, s_date_buf);
  }
}

static void layout_layers(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);

  // Margins: a bit larger on round to avoid the curved edge
  const int16_t margin = PBL_IF_ROUND_ELSE(14, 6);

  // Heights
  const int16_t time_h = 42;
  const int16_t date_h = 32;
  const int16_t bottom_y_time = bounds.size.h - margin - time_h;
  const int16_t bottom_y_date = bounds.size.h - margin - date_h;

#if defined(PBL_ROUND)
  // ROUND: time centered at bottom, no date
  GRect time_frame = GRect(margin, bottom_y_time, bounds.size.w - 2 * margin, time_h);
  layer_set_frame(text_layer_get_layer(s_time_layer), time_frame);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Hide date on round
  if (s_date_layer) {
    layer_set_hidden(text_layer_get_layer(s_date_layer), true);
  }
#else
  // RECTANGULAR: time bottom-right, date bottom-left
  const int16_t half_w = bounds.size.w / 2;

  // Date (left bottom)
  if (!s_date_layer) {
    s_date_layer = text_layer_create(GRect(0, 0, 10, 10));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorBlack);
    text_layer_set_overflow_mode(s_date_layer, GTextOverflowModeWordWrap);
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentLeft);
    text_layer_set_font(s_date_layer, s_date_font);
    layer_add_child(root, text_layer_get_layer(s_date_layer));
  }
  GRect date_frame = GRect(margin, bottom_y_date, half_w - 2 * margin, date_h);
  layer_set_frame(text_layer_get_layer(s_date_layer), date_frame);
  layer_set_hidden(text_layer_get_layer(s_date_layer), false);

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
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MICRO_20));

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