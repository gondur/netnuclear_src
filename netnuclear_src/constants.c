 int country_positions[] = {
   38,  38,
   6,   216,
   392, 70,
   392, 216,
   192, 162,
   -1
};

/* add values from player_positions[] to these */
 int city_positions[] = {
   14, 32, 122, 0,  212, 4,   2,   88,  126, 114,
   2,  54, 128, 22, 2,   126, 140, 116, 206, 122,
   90, 2,  26,  36, 106, 88,  188, 72,  212, 124,
   96, 16, 58,  56, 206, 26,  54,  126, 202, 116,
   68, 0,  38,  50, 142, 54,  68,  86,  136, 96,
   -1
};

const char *leader_names[] = {
   "Ronnie Raygun",
   "Infidel Castro",
   "Jimi Farmer",
   "Col. Khadaffy",
   "Tricky Dick",
   "Kookamamie",
   "Gorbachef",
   "P.M. Satcher",
   "Mao The Pun",
   "Ghanji",
   ""
};

const char *weapon_names[] = {
   " 10MT Missile",
   " 20MT Missile",
   " 50MT Missile",
   "100MT Missile",
   " 10MT Warhead",
   " 20MT Warhead",
   " 50MT Warhead",
   "100MT Warhead",
   " NP-1 Bomber",
   " GR-2 Bomber",
   " L.N.D.S.",
   " MegaCannon",
   ""
};

const char *missile_names[] = {
   "Carnage missile",
   "Gravedigger missile",
   "Deathbringer missile",
   "Pacifier missile",
   ""
};

const char *bomber_names[] = {
   "Nuclear Postman bomber",
   "Grim Reaper bomber",
   ""
};

 int megatons[] = {
   10,
   20,
   50,
   100,
   -1
};

/* damage done by nukes */
 int damage[] = {
   2,  12,	/*	10 mt	*/
   5,  19,	/*	20 mt	*/
   10, 24,	/*	50 mt	*/
   25, 36,	/*	100 mt	*/
   -1
};

const char *msgs_build[] = {
   "%s prepares for war.",
   "%s produces weapons.",
   "%s creates instruments of destruction.",
   "%s builds nuclear toys.",
   "%s's people add to their arsenal.",
   ""
};

const char *msgs_build_result[] = {
   "Your factories produced:",
   "Your citizens made:",
};

const char *msgs_propaganda[] = {
   "%s conducts propaganda towards %s.",
   "%s blitzs %s's people with lies.",
   "%s invites %s's people to a BBQ.",
   "%s has a concert for %s's people.",
   ""
};

const char *msgs_prepare_missile[] = {
   "%s readies a lone missile.",
   "%s prepares a missile for launch.",
   "%s puts a missile on the launch pad.",
   "%s prepares a missile strike.",
   ""
};

const char *msgs_prepare_bomber[] = {
   "%s deploys a bomber.",
   "%s puts a bomber on the runway.",
   "%s fuels up a bomber.",
   "%s scrambles a bomber.",
   ""
};

const char *msgs_missile_attack[] = {
   "%s Launches a missile at %s.",
   "%s delivers a nuclear blow to %s.",
   "%s targets %s for destruction.",
   ""
};

const char *msgs_bomber_attack[] = {
   "%s Drops the Bomb on %s!",
   "%s sends a warhead to %s!",
   "%s delivers death to %s!",
   "%s rains flaming death on %s!",
   ""
};

const char *msgs_nuke[] = {
   "Fireball engulfs city: %d million killed.",
   "Fireball engulfs city: CITY DESTROYED",
   "Nuclear explosion kills %d million.",
   "Nuclear explosion destroys city",
   "Radioactive fallout kills %d million.",
   "Radioactive fallout destroys city",
   "Radioactive Beta-Rays kills %d million.",
   "Radioactive Beta-Rays destroy city",
   "Lethal Gamma-Rays kills %d million.",
   "Lethal Gamma-Rays destroys city",
   "Dirty bomb kills %d million.",
   "Dirty bomb destroys city",
   ""
};

const char *msgs_defend[] = {
   "%s deploys a defense system.",
   "%s assumes a defensive posture.",
   "%s prepares for impact.",
   "%s initiates a strategic defense.",
   ""
};

const char *msgs_welcome[] = {
   "Good Luck!",
   "Best Wishes!",
   "Let the game begin!",
   "The end is near.",
   "The coin is tossed.",
   "The die is cast.",
   "Survive, if you can.",
   "Diplomacy Terminal #1.",
   "Your opponents await.",
   "Let the contest begin.",
   "Dawn is in 32 minutes.",
   "Everyone is here.",
   "All eyes are upon you.",
   "Do you have insurance?",
   "Your destiny awaits.",
   "Thermo Nuclear war?",
   "Alert stats: RED.",
   "All wings standing by.",
   "All systems are go.",
   "Are you ready?",
   "Can you take the heat?",
   "Got what it takes?",
   "Think you can survive?",
   "Go for the Hill!",
   "You have 1 credit.",
   "You don't have a chance.",
   "Can you get Highscore?",
   "Millions count on you.",
   "No turning back now.",
   "Think you can win?",
   "Can you save Cron?",
   "Good Morning.",
   "Good Afternoon.",
   "Good Evening.",
   "Good Day.",
   "Good Bye.",
   "I am fully functional.",
   "Terminal mode enabled.",
   "Terminal active.",
   "Everything is lit.",
   "Power up complete.",
   "Status: Green.",
   "This space for lease.",
   "Sequence initiated.",
   "Welcome aboard, Captain.",
   "All stations ready.",
   "You have the Con.",
   "Nuke em till they glow.",
   ""
};

const char *msgs_leaders[] = {
   "I consulted my astrologer.",
   "Nancy! Oh Nancy!",
   "I want to press the button.",
   "I need a nap.",
   "Hug me with nuclear arms.",
   "I use awesome firepower.",
   "Who says I'm moody?",
   "I will reclaim our islands!",
   "My nation will prevail.",
   "I hate two-bit nations.",
   "What? Me an Iron Maiden?",
   "Would you like a cigar?",
   "Nukes stink!",
   "What missile crisis?",
   "Join me! Have a cigar.",
   "We are like neighbors.",
   "Would you like some refugees?",
   "Who's the infidel?",
   "Have a cookie my friend.",
   "My friends are my enemies.",
   "Now, Did I like you or not?",
   "I just want to help.",
   "Who says I'm crazed?!",
   "Just call me 'The Colonel'.",
   "Fitted sheets for everyone.",
   "Persecute dry cleaners!",
   "Fanatic! Me. HAHAHAHAHA!",
   "You'd look good in towels.",
   "Toga! Toga! Toga!",
   "Join me or die!",
   "I propogate better!",
   "This is pun!",
   "Be nice to me.",
   "Who me care?",
   "Care for a fortune cookie.",
   "Bide your time.",
   "It's all peanuts to me.",
   "Let's sign an accord.",
   "I'm everyones friend.",
   "Be nice to everyone!",
   "Propaganda has its limits.",
   "Help me!",
   "Lies..What Lies?",
   "Tapes..What Tapes?",
   "I am not a crook!",
   "I've been .. Never mind.",
   "Trust me!",
   "I'd never lie to You!",
   "Lies..Lies..Sweet little Lies.",
   "Love me or leave me!",
   "Glasnost or Bust!",
   "I'll reform, I promise!",
   "Die a horrible death!",
   "Your system is doomed!",
   "There is no need for struggle.",
   "Prosper through non-violence.",
   "Don't mess with 'The Ghanj'!",
   "No body props better!",
   "My prop never fails!",
   "Don't even think of nuking me.I forgot..There can be only one!",
   "Blessed are the meek!",
   "Worse, how could it be worse.",
   "I am in control.",
   "Danger! Danger!",
   "We can rebuild it.",
   "Does my city glow at night?",
   "Foul!!!! He fouled me!!!",
   "Who's your buddy? I am!",
   "Ouch that hurt!",
   "Leave me alone.",
   "I don't want to talk.",
   "Who turned out the lights.",
   "Change your happy face!",
   "Smile at me!",
   "Frown at him!",
   "I'm flailing!",
   "Help! I need your help!",
   "Without you I will be killed!",
   "They want me to attack you.",
   "I'm so happy!",
   "I'm in over my head.",
   "You are truly feeble.",
   "You are a lucky lucky man.",
   "Propaganda usually fails.",
   "Its the only way to be sure.",
   "Nukes yield more.",
   "Nukes are risky business.",
   "I want the stork!",
   "Anything is better then an L5.",
   "Sky 'Scrapers are invincible.",
   "100 Megs or nothing!",
   "Can't take the Heat?",
   "Are you my friend?",
   "Be my friend.",
   "That was a poor move.",
   "Oh! Do it again! Please!",
   "Ah......No!",
   "Stay On Target!",
   "I'm Batman.",
   "Who is this Batman Fellow?",
   "Savoir Faire is everywhere.",
   "Here I come to save the day!",
   "I won't change my mind!",
   "Always expect the Unexpected.",
   "Lets work together.",
   "My dislike for you grows!",
   "Hey leave that city alone!",
   "Get your eyes off my city!",
   "Bombers use multiple payloads.",
   "Missiles are easier to kill.",
   "Mega Cannons kill all!",
   "100 Megatons hurts bad!",
   "Attack everyone.",
   "Anger no one.",
   "Everyone has a favorite city.",
   "Warmongers are easy.",
   "Never rely on chaos!",
   "400 Million will never happen.",
   "Get a highscore, win A prize.",
   "I am the Puppet Master.",
   "Hey? Who's whistling?",
   "Whos bad?",
   "May I help you?",
   "NW = ELH + JVC.",
   "Seen any good weights lately?",
   "Are aliens illegal?",
   "Pass the cow.",
   "Have any undesireables?",
   "Ever play Might And Magic?",
   "Send in your Registration card.",
   "Suggestions? Write us a letter.",
   "I've done worse than kill you.",
   "I've hurt you.",
   "Revenge is Best served cold.",
   "Well....I don't remember.",
   "Are we having fun yet?",
   "Play a game, get a message.",
   "Play a turn, get a message.",
   "Incoming!",
   "Fire in the hole!",
   "I'll be back!",
   "I know his name!",
   "What he said.",
   "I concur.",
   "Nuke 'em till they glow.",
   "Never mind.",
   "Go Goobers!",
   "I'm moving to L5.",
   "Kill the human!",
   "Don't know my own strength!",
   "Lets you and him fight!",
   "What a country!",
   "You MUST be kidding.",
   "Yes, I am picking on you.",
   "What goes around, comes around.",
   "The end is near!",
   "Whos your pal?",
   "The world will end in 5 minutes.",
   "That was a fatal mistake.",
   "You are the Evil Empire!",
   "It's bedtime for you, bozo!",
   "Run, but you can't hide!",
   "Ready the Royal Navy!",
   "Prepare for war!",
   "We are not amused.",
   "Die Infidel!",
   "Oops, Nevermind.",
   "I am really mad at you.",
   "That burns my embers.",
   "Aha! I Knew it! Treachery!",
   "You crossed the Line of Death!",
   "I hope you like sheets.",
   "Time for another strike!",
   "You are The Great Evil!",
   "May all your nukes dud!",
   "That was not funny, enemy mine!",
   "My horde will get you!",
   "I will kill you with puns!",
   "I know how to handle threats!",
   "Please don't hurt me again.",
   "Oh no! Not the peanut farm!",
   "And they say I lie!",
   "That was an accident, Right?",
   "But we had an understanding!",
   "I wanted to be your friend!",
   "I will crush you!",
   "Come on! Trust me!",
   "Now I am distressed!",
   "Prepare to eat propaganda!",
   "That hurt you more than me.",
   "Stay on target!",
   "Go for the hill!",
   "Its better to burn out...",
   "I give up.",
   "You my friend, are hopeless.",
   "Barbarian.",
   "You are a nit.",
   "I bet you think thats funny!",
   "Not even a scratch!",
   "How dare you!",
   "It is time for retribution!",
   "You will pay for that.",
   "Thou shalt not win.",
   "I'll get you yet!",
   "Did you attack me?",
   "Please spare me!",
   "Mercy...Mercy!",
   "Now I am really mad!",
   "Was that really necessary.",
   "I will crush you like a bug!",
   "You couldn't harm a fly.",
   "That will hasten your fall.",
   "HAHAHAHAHAHAHA.",
   "I don't need that city anyway.",
   "You lied to me!",
   "Hrumph, Well I never!",
   "Are you my freind?",
   "You will die for that!",
   "Laugh now monkey boy!",
   "Time out! Time out!",
   "Leave me alone!",
   "Why'd you pick on me!?",
   "You dirty Rat!",
   "You'll pay for that.",
   "You shall be punished.",
   "I'll get you yet!",
   "Not even a scratch!",
   "Didn't bother me!",
   "Now you've got me mad.",
   "Go away!",
   "I WILL kill you!",
   "Watch it!",
   "I'm Batman.",
   "Always expect the unexpected!",
   "I expected that from you!",
   "You are lower than low!",
   "I should have known!",
   "'Tis A flesh wound!",
   "Nuke them instead!",
   "Prop them instead.",
   "No! Build! The end is near!",
   "My final strike will get you!",
   "My power is beyond you.",
   "We can still be friends.",
   "So this is what has come to!",
   "Draw!",
   "I know something you don't.",
   "Prepare to meet your maker.",
   "You should floss more often.",
   "Fire the Euclid Cannon!",
   "My ghost will haunt you.",
   "This is your last chance!",
   "Revenge is best served cold.",
   "Are we having fun yet?",
   "I'll be back!",
   "That was a fatal mistake.",
   ""
};