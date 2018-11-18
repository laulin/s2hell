#ifndef __S2HELL_COMMANDS_HEADER__
#define __S2HELL_COMMANDS_HEADER__

#define man_lscmd "name\n\
\tlscmd\n\
synopsis\n\
\tlscmd\n\
descripton\n\
\tDisplay the list of available commands\n"

s2hell_code default_lscmd(uint8_t argc, s2hell_argument* argv);

#define man_man "name\n\
\tman\n\
synopsis\n\
\tman COMMAND\n\
descripton\n\
\tDisplay a command help\n"

s2hell_code default_man(uint8_t argc, s2hell_argument* argv);

#define man_search "name\n\
\thelp\n\
synopsis\n\
\tsearch PATTERN\n\
descripton\n\
\tSearch a string in stdin\n"

s2hell_code default_search(uint8_t argc, s2hell_argument* argv);

#define man_echo "name\n\
\thelp\n\
synopsis\n\
\techo TEXT\n\
descripton\n\
\tPush the TEXT argument to stdout\n"

s2hell_code default_echo(uint8_t argc, s2hell_argument* argv);

#endif