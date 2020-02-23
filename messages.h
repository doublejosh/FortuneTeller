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
		"     WiFi Error,   ",
		"     restart.      ",
		"*                  *"
	}, {
		"|-                -|",
		"|--   Hello :)   --|",
		"|---            ---|",
		"|------------------|"
	}, {
		"*                  *",
		"   Our meeting has  ",
		"     been saved.    ",
		"*                  *"
	}, {
		"*                  *",
		"   Future fortunes  ",
		"    now adjusted.   ",
		"*                  *"
	}, {
		"   Use buttons to   ",
		"  answer questions. ",
		"                    ",
		"<<< NO      YES >>> "
	}, {
		"*                  *",
		"    Random choice   ",
		"       picked.      ",
		"*                  *"
	}, {
		"                    ",
		"  Was your fortune  ",
		"     on target?     ",
		"                    "
	}};

char FORTUNES[][HEIGHT][WIDTH+1] = {{
    "A hunch is just     ",
    "creativity trying to",
    "tell you something. ",
		"                    "
  }, {
		"A soft voice may be ",
		"awfully persuasive. ",
		"                    ",
		"                    "
  	}, {
		"Adventure can be    ",
		"real happiness.     ",
		"                    ",
		"                    "
	}, {
		"At the touch of love",
		"everyone becomes a  ",
		"poet.               ",
		"                    "
	}, {
		"A friend is a       ",
		"present you give    ",
		"yourself.           ",
		"                    "
	}};
