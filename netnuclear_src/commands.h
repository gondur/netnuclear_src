#define COMMAND(name)		void name(char *args)

/* functions */
void do_command(char *str);
COMMAND(cmd_add);
COMMAND(cmd_clear);
COMMAND(cmd_close);
COMMAND(cmd_connect);
COMMAND(cmd_disconnect);
COMMAND(cmd_help);
COMMAND(cmd_host);
COMMAND(cmd_name);
COMMAND(cmd_quit);
COMMAND(cmd_start);
COMMAND(cmd_turn);
COMMAND(cmd_version);
COMMAND(cmd_who);
