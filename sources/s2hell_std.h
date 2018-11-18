#ifndef __S2HELL_STD_HEADER__
#define __S2HELL_STD_HEADER__

typedef struct _std
{
    char buffer[S2HELL_STD_SIZE];
    uint8_t position;
}
s2hell_std;

void s2hell_reset_stds();
void s2hell_clear_stdout();
uint8_t s2hell_get_current_stdout();
uint8_t s2hell_get_current_stdin();
void s2hell_swap_stds();
char * s2hell_get_current_stdout_buffer(uint8_t *);
void s2hell_stdout_report_write(uint8_t);
void s2hell_stdout_nwrite(const char * source, uint8_t source_size);
void s2hell_stdout_write(const char * source);
const char * s2hell_get_complet_stdout_buffer();
const char * s2hell_get_complet_stdin_buffer();
uint8_t s2hell_get_stdin_size();

#endif