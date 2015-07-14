#include <pebble.h>

Window* window;
MenuLayer *menu_layer;
int send_counter = 0;

enum {
  KEY_ON = 1,
  KEY_OFF = 0,
  KEY_ANLAGE = 5,
  KEY_LAMPE = 6,
  KEY_STATUS = 11,
  KEY_DEVICE = 12
};

void send_int(uint8_t key, uint8_t device)
{
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    dict_write_uint8(iter, KEY_STATUS , key);
    dict_write_uint8(iter, KEY_DEVICE, device);
    dict_write_end(iter);
 
    AppMessageResult err_app_send = app_message_outbox_send();
    //APP_LOG(APP_LOG_LEVEL_INFO, "Send result: %u ", err_app_send);
    if (err_app_send != 0)
    {
      send_counter++;
      APP_LOG(APP_LOG_LEVEL_INFO, "Send counter %d", send_counter);
      if (send_counter < 20) {
        send_int(key, device);
      }
    }
    else {
      send_counter = 0;
    }
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
  vibes_short_pulse();
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}


void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    //Which row is it?
    switch(cell_index->row)
    {
    case 0:
        menu_cell_basic_draw(ctx, cell_layer, "Anlage", "Kurz: an / Lang: aus", NULL);
        break;
    case 1:
        menu_cell_basic_draw(ctx, cell_layer, "Lampe WZ", "Kurz: an / Lang: aus", NULL);
        break;
    }
}
 
uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
  return 2; 
}
 
void select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    //Get which row
    int which = cell_index->row;

  send_int(1, which);
  //if (which == 0){ send_int(KEY_ON,KEY_ANLAGE);}
  
  //if (which == 1){ send_int(KEY_ON,KEY_LAMPE);}
  //send_message();

}

void select_long_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    //Get which row
    int which = cell_index->row;

  send_int(0, which);
  //if (which == 0){ send_int(KEY_OFF,KEY_ANLAGE);}
  
  //if (which == 1){ send_int(KEY_OFF,KEY_LAMPE);}
    //send_int(KEY_OFF,which);

}

void window_load(Window *window)
{
    //Create it - 12 is approx height of the top bar
    menu_layer = menu_layer_create(GRect(0, 0, 144, 168 - 16));
 
    //Let it receive clicks
    menu_layer_set_click_config_onto_window(menu_layer, window);
 
    //Give it its callbacks
    MenuLayerCallbacks callbacks = {
        .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
        .select_click = (MenuLayerSelectCallback) select_click_callback,
        .select_long_click = (MenuLayerSelectCallback) select_long_click_callback 
    };
    menu_layer_set_callbacks(menu_layer, NULL, callbacks);
 
    //Add to Window
    layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
}
 
void window_unload(Window *window)
{
   menu_layer_destroy(menu_layer);
}
 
void init()
{
    window = window_create();
    WindowHandlers handlers = {
        .load = window_load,
        .unload = window_unload
    };
    window_set_window_handlers(window, (WindowHandlers) handlers);
  
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());    //Largest possible input and output buffer sizes
    window_stack_push(window, true);
}
 
void deinit()
{
    window_destroy(window);
}
 
int main(void)
{
    init();
    app_event_loop();
    deinit();
}