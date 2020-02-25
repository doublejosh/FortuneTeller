#define WIDTH 20
#define HEIGHT 4

#define GREET1 0
#define GREET2 1
#define BOOT 3
#define SAVE 4
#define FUTURE 5
#define GREET3 6
#define RANDOM 7
#define ACCURATE 8
#define CORRECT 9
#define WRONG 10
#define TIMEOUT 11
#define FETCHING 12
#define FREEBIE 13
#define PICK_YES 14
#define PICK_NO 15

char MESSAGES[][HEIGHT][WIDTH+1] = {{
		"*                  *",
		"  Contemplate your  ",
		"  uncertainty...    ",
		"*                  *"
	}, {
		"*                  *",
		"   I wonder what    ",
		"  you're wondering? ",
		"*                  *"
	}, {
		"*                  *",
		"     WiFi Error,    ",
		"     restart.       ",
		"*                  *"
	}, {
		"                    ",
		"      Hello :)      ",
		"                    ",
		"                    "
	}, {
		"*                  *",
		"   Our meeting is   ",
		"    being saved.    ",
		"*                  *"
	}, {
		"*                  *",
		"   Future fortunes  ",
		"    now adjusted.   ",
		"*                  *"
	}, {
		"*  Use buttons to  *",
		"  answer questions. ",
		"                    ",
		"<<< NO      YES >>> "
	}, {
		"   Random choice.   ",
		"                    ",
		"       _====_       ",
		"  q  8( 0..0 )8  p  "
	}, {
		"                    ",
		"  Was your fortune  ",
		"     on target?     ",
		"                    "
	}, {
		"                    ",
		"  Oh good, thanks!  ",
		"                    ",
		"   I'll remember.   "
	}, {
		"                    ",
		"      Huh, ok.      ",
		"                    ",
		"   I'll remember.   "
	}, {
		"                    ",
		"       Hello?       ",
		"                    ",
		"    Another coin?   "
	}, {
		"!                  !",
		"    Calibrating     ",
		"    your fortune.   ",
		"!                  !"
	}, {
		"   A freebie!? OK   ",
		"                    ",
		"       _====_       ",
		"  q  8( 0..0 )8  p  "
	}, {
		"                    ",
		"   You picked YES.  ",
		"                    ",
		"                    "
	}, {
		"                    ",
		"   You picked NO.   ",
		"                    ",
		"                    "
	}};
