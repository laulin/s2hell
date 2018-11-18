#ifndef __S2HELL_TOKENIZER_HEADER__
#define __S2HELL_TOKENIZER_HEADER__

#define m_is_number(c) (c>= 48 && c <= 57)
#define m_is_upper_letter(c) (c >= 65 && c <= 90)
#define m_is_lower_letter(c) (c >= 97 && c <= 122)
#define m_is_w(c) (m_is_upper_letter(c) || m_is_lower_letter(c) || (c == 95))
#define m_is_w_and_d(c) (m_is_w(c) || m_is_number(c))


typedef enum _token_type
{
    eol=0,
    word,
    command,
    not_defined,
    bad_token,
    pipe
}
s2hell_token_type;

// contains each token of a command line
typedef struct _token
{
    uint8_t position;
    uint8_t length;
    s2hell_token_type type;
}
s2hell_token;

int8_t s2hell_tokenize(const char* line, s2hell_token* token, uint8_t token_size);
void s2hell_show_tokens(const char* line, s2hell_token* token, uint8_t token_size);

void jump_space(const char* string, uint8_t * position);
int8_t token_word(const char* string, uint8_t position, uint8_t * length);
int8_t token_quotes(const char* string, char quote, uint8_t * position, uint8_t * length);


#endif