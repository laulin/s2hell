#ifndef __S2HELL_TERMINAL_HEADER__
#define __S2HELL_TERMINAL_HEADER__

#define S2HELL_TERMINAL_BUFFER_SIZE 80

void s2hell_clear_line();
uint8_t* s2hell_get_line_buffer();
uint8_t s2hell_get_line_size();
uint8_t s2hell_add_character(uint8_t);
void s2hell_move_cursor_backward(uint8_t n);
void s2hell_move_cursor_forward(uint8_t n);
void s2hell_backspace();
void s2hell_delete();
void s2hell_process_terminal(uint8_t x);

// this function is called  for every print buffer action
void set_print_buffer(void (*print_buffer)(const uint8_t* buffer, uint8_t size));
// this function is called for each print char action
void set_print_char(void (*print_char)(uint8_t c));
// when return is hit, this function is called to process the line
void set_on_new_line(void (*on_new_line)(const uint8_t * buffer, uint8_t size));
// on tab this function is call (for completion)
void set_on_tab(void (*on_tab)(const uint8_t * buffer, uint8_t size));

// can be called to report stdout
void call_print_buffer(const char * buffer, uint8_t size);

#endif

