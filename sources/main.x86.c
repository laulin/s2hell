#include "s2hell.h"
#include <stdio.h>

#define MAX_TOKEN 32

// thank you http://patorjk.com for the generator
const char * HEADER = "\n \
   ________   __  __     ____\n \
  / ___/__ \\ / / / /__  / / /\n \
  \\__ \\__/ // /_/ / _ \\/ / / \n \
 ___/ / __// __  /  __/ / /  \n \
/____/____/_/ /_/\\___/_/_/   \n \
                             \n \
\n";

int main(int argc, char* argv[])
{
    if (argc > 1)
    {
        printf("%sX86 Environment - 18/11/2018\n\n", HEADER);
        printf("infos :\nargs : \"%s\"\n", argv[1]);
        s2hell_token tokens[MAX_TOKEN] = {0};
        s2hell_set_default_x86_commands();
        int token_number = s2hell_tokenize(argv[1], tokens, MAX_TOKEN);
        printf("Number of token found : %d\n", token_number);
        if (token_number > 0)
        {
            s2hell_show_tokens(argv[1], tokens, MAX_TOKEN);
            printf("\n--- Now let's run ! ---\n\n");
            s2hell_code exitstatus = s2hell_execute(argv[1], tokens, MAX_TOKEN);
            printf("%s", s2hell_get_complet_stdout_buffer());
            printf("\nexitstatus : %d\n", exitstatus);
        }
    }
    else
    {
        printf("No args\n");
    }
    return 0;
}