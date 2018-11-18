#include "s2hell.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// It jumps space to get next world [thank you Jerome P. ;)] 
void jump_space(const char* string, uint8_t * position)
{
    while(string[*position] == ' ')
    {
        (*position)++;
    }
}

// Get any word which contains [a-zA-Z_0-9]
int8_t  token_word(const char* string, uint8_t position, uint8_t * length)
{
    if (!m_is_w_and_d(string[position]))
    {
        return ERROR;
    }

    while(m_is_w_and_d(string[position + *length]))
    {
        (*length)++;
    }
    return NO_ERROR;
}

// From a string, extract a quoted set of caracter.
// Manage escaped quotes (\" or \' for example)
int8_t token_quotes(const char* string, char quote, uint8_t * position, uint8_t * length)
{
    while(string[*position + *length] != 0)
    {
        if (*length == 0)
        {
            (*length)++;
            continue;
        }

        if(string[*position + *length] == quote && string[*position + *length -1] != 92)
        {
            (*position)++;
            (*length)--;
            return NO_ERROR;
        } 

        (*length)++;     
    }
    return ERROR;
}

// basic function to init an array of token
// not to be tested
static void _s2hell_init_tokens(s2hell_token* token, uint8_t token_size)
{
    for(uint8_t i=0; i<token_size; i++)
    {
        token[i].position = 0;
        token[i].length = 0;
        token[i].type = eol;
    }
}

// Very conveniant for debugging and understanding why test fail
void s2hell_show_tokens(const char* line, s2hell_token* token, uint8_t token_size)
{
    for(uint8_t i=0; i<token_size; i++)
    {
        printf("token %d\n", i);
        printf("\tposition %d\n", token[i].position);
        printf("\tlength %d\n", token[i].length);
        printf("\ttype %d\n", token[i].type);
        if(token[i].length > 0)
        {
            printf("\tvalue '%.*s'\n", token[i].length, &line[token[i].position]);
        }
        if (token[i].type == eol || token[i].type == bad_token)
        {
            return;
        }
    }
}

// Manages the behavior of the tokenizer when a simple quote  or a doubple quote is detected
int8_t s2hell_tokenize_quoted(const char* line, uint8_t* position, s2hell_token* token)
{
    uint8_t length = 0;
    int8_t code = token_quotes(line, line[*position], position, &length);
    // this case happen if the quoting is not ended
    if (code != NO_ERROR)
    {
        //parsing error
        token->position = *position;
        token->length = length;
        token->type = bad_token;
        return ERROR;
    }
    token->type = word;
    token->position = *position;
    token->length = length;
    *position = *position + length + 1; // +1 is to complet the token_quotes behaviour

    return NO_ERROR;
}

// Manages the behavior of the tokenizer for a word
int8_t s2hell_tokenize_word(const char* line, uint8_t* position, s2hell_token* token)
{
    uint8_t length = 0;
    int8_t code = token_word(line, *position, &length);
    // this case happen if the quoting is not ended
    if (code != NO_ERROR)
    {
        //parsing error
        token->position = *position;
        token->length = length;
        token->type = bad_token;
        return ERROR;
    }

    token->position = *position;
    token->length = length;
    *position = *position + length ;

    return NO_ERROR;
}


// Generates tokens using the line provided
// We apologize for the function's number of line
int8_t s2hell_tokenize(const char* line, s2hell_token* token, uint8_t token_size)
{
    uint8_t position = 0; // current position in the string
    s2hell_token_type previsous_type = not_defined; // What was the previous token, use to tag current token

    _s2hell_init_tokens(token, token_size); // reset the token list

    for(uint8_t i=0; i<token_size; i++)
    {

        jump_space(line, &position);
        switch(line[position])
        {
            // if end of line or end of string is detected
            case 0:
            case 10:
                if (previsous_type == pipe)
                {
                    //parsing error
                    token[i].position = position;
                    token[i].length = 1;
                    token[i].type = bad_token;
                    return ERROR;
                }
                token[i].type = eol;
                previsous_type = eol;
                return i+1;
            
            // if quote (' or ") ar detected
            case 34:
            case 39:
                {
                    int8_t code = s2hell_tokenize_quoted(line, &position, &token[i]);
                    if (code != NO_ERROR)
                    {
                        return ERROR;
                    }
                    previsous_type = word;
                }
                break;

            case 124: // pipe char
                if (previsous_type == pipe || previsous_type == not_defined)
                {
                    //parsing error
                    token[i].position = position;
                    token[i].length = 1;
                    token[i].type = bad_token;
                    return ERROR;
                }
                token[i].position = position;
                token[i].length = 1;
                token[i].type = pipe;
                position++;
                previsous_type = pipe;
                break;

            // for other characters
            default:
                {
                    int8_t code = s2hell_tokenize_word(line, &position,&token[i]);

                    if (code != NO_ERROR)
                    {
                        return ERROR;
                    }

                    if (previsous_type == not_defined || previsous_type == pipe)
                    {
                        token[i].type = command;
                        previsous_type = command;
                    }
                    else
                    {
                        token[i].type = word;
                        previsous_type = word;
                    }

                }
                break;
        }

    }

    return ERROR;
}


