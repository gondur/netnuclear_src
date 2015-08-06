#define BUFFER_SIZE	1024
#define PI	3.14159265f

#define NUM_LEADERS		10
#define LEADER_RONNIE		0
#define LEADER_INFIDEL		1
#define LEADER_JIMI		2
#define LEADER_KHADAFFY		3
#define LEADER_TRICKYDICK	4
#define LEADER_KOOKAMAMIE	5
#define LEADER_GORBACHEF	6
#define LEADER_SATCHER		7
#define LEADER_MAO		8
#define LEADER_GHANJI		9

/* weapons */
#define NUM_WEAPONS		12
#define WEAPON_MISSILE_10MT	0
#define WEAPON_MISSILE_20MT	1
#define WEAPON_MISSILE_50MT	2
#define WEAPON_MISSILE_100MT	3
#define WEAPON_WARHEAD_10MT	4
#define WEAPON_WARHEAD_20MT	5
#define WEAPON_WARHEAD_50MT	6
#define WEAPON_WARHEAD_100MT	7
#define WEAPON_NP1		8
#define WEAPON_GR2		9
#define WEAPON_LNDS		10
#define WEAPON_MEGA		11

#define MEGATON_10	0
#define MEGATON_20	1
#define MEGATON_50	2
#define MEGATON_100	3

extern  int country_positions[];
extern  int city_positions[];
extern const char *leader_names[];
extern const char *weapon_names[];
extern const char *missile_names[];
extern const char *bomber_names[];
extern  int megatons[];
extern  int damage[];
extern const char *msgs_build[];
extern const char *msgs_build_result[];
extern const char *msgs_propaganda[];
extern const char *msgs_prepare_missile[];
extern const char *msgs_prepare_bomber[];
extern const char *msgs_missile_attack[];
extern const char *msgs_bomber_attack[];
extern const char *msgs_nuke[];
extern const char *msgs_defend[];
extern const char *msgs_welcome[];
extern const char *msgs_leaders[];
