/* functions */
void do_ai();
void ai_select_target_city(int player, struct action_node *action);
int ai_use_defense(int player, int type);
int has_warhead(int player, int mt);
void send_ai_actions();
void ai_clear_messages();
void ai_random_message(int player);
