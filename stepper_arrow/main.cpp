// GReads press of up/down arrow keys and notifies Arduino over serial
// Remember to compile with lncurses and lserial
// This is stupid, might want to add functionality to 
// input an integer RMP, hit enter, send that instead. 

#include <stdio.h>
#include <ncurses.h>
#include <SerialStream.h>
#include <ctype.h>

#define PORT "/dev/ttyACM0" //May be ttyACM1 - check Arduino

using namespace LibSerial;
using namespace std;

SerialStream ardu;
char * intprtkey(int ch);
const int BUFFER_SIZE = 4;
char output_buffer[BUFFER_SIZE];

int main(void) {

    int pps = 0;
    int dir = 1;

    // Init and configure serial comm
    ardu.Open(PORT);    
    ardu.SetBaudRate(SerialStreamBuf::BAUD_9600);  
    ardu.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);

    WINDOW * mainwin;
    int ch;

    /*  Initialize ncurses  */
    if ( (mainwin = initscr()) == NULL ) {
		fprintf(stderr, "Error initializing ncurses.\n");
		exit(EXIT_FAILURE);
    }

    noecho();                  /*  Turn off key echoing                 */
    keypad(mainwin, TRUE);     /*  Enable the keypad for non-char keys  */

    /*  Print a prompt and refresh() the screen  */
    mvaddstr(5, 10, "Press up to increase speed, down to decrease speed, 0 to set to 0, 'q' to quit.");
    mvprintw(7, 10, "You pressed: ");
    refresh();

    /*  Collect keyboard inputs until user presses 'q'  */
    while ( (ch = getch()) != 'q' ) {

		/*  Delete the old response line, and print a new one  */
		deleteln();
		mvprintw(7, 10, "You pressed: 0x%x (%s)", ch, intprtkey(ch));
		//mvprintw(8,10, "%d", ch);
		refresh();

		switch(ch) {
    		case 258 :
				mvprintw(9, 10, "Decrease speed by 1 PPS.");
				refresh();
                pps--;
           	break;

			case 259: 
				mvprintw(9, 10, "Increase speed by 1 PPS.");
				refresh();
                pps++;
			break;

			case 48:
				mvprintw(9, 10, "Reset speed to 0 PPS.");
				refresh();
                pps = 0;
			break;

			case 260:
				mvprintw(9, 10, "Set reverse.");
				refresh();
                dir = 0;
			break;

			case 261:
				mvprintw(9, 10, "Set forward.");
				refresh();
                dir = 1;
			break;
		}

        if (pps < 0) {
            pps = 0;
        }
        
        output_buffer[0] = pps & 0xff;
		output_buffer[1] = (pps >> 8) & 0xff;
		output_buffer[2] = dir & 0xff;
		output_buffer[3] = (dir >> 8) & 0xff;
		ardu.write(output_buffer, BUFFER_SIZE);

        mvprintw(8,10, "Speed = %d, direction = %d", pps,dir);
		refresh();
    }


    /*  Clean up after ourselves  */
    delwin(mainwin);
    endwin();
    refresh();

    return EXIT_SUCCESS;
}


/*  Struct to hold keycode/keyname information  */
struct keydesc {
    int  code;
    char name[20];
};


/*  Returns a string describing a character passed to it  */
char * intprtkey(int ch) {

    /*  Define a selection of keys we will handle  */
    static struct keydesc keys[] = { 
					 { KEY_UP,        "Up arrow"        },
				     { KEY_DOWN,      "Down arrow"      },
				     { KEY_LEFT,      "Left arrow"      },
				     { KEY_RIGHT,     "Right arrow"     },
				     { KEY_HOME,      "Home"            },
				     { KEY_END,       "End"             },
				     { KEY_BACKSPACE, "Backspace"       },
				     { KEY_IC,        "Insert"          },
				     { KEY_DC,        "Delete"          },
				     { KEY_NPAGE,     "Page down"       },
				     { KEY_PPAGE,     "Page up"         },
				     { KEY_F(1),      "Function key 1"  },
				     { KEY_F(2),      "Function key 2"  },
				     { KEY_F(3),      "Function key 3"  },
				     { KEY_F(4),      "Function key 4"  },
				     { KEY_F(5),      "Function key 5"  },
				     { KEY_F(6),      "Function key 6"  },
				     { KEY_F(7),      "Function key 7"  },
				     { KEY_F(8),      "Function key 8"  },
				     { KEY_F(9),      "Function key 9"  },
				     { KEY_F(10),     "Function key 10" },
				     { KEY_F(11),     "Function key 11" },
				     { KEY_F(12),     "Function key 12" },
				     { -1,            "<unsupported>"   }
    };

    static char keych[2] = {0};
    
    if ( isprint(ch) && !(ch & KEY_CODE_YES)) {

		/*  If a printable character  */
		keych[0] = ch;
		return keych;
    }

    else {

		/*  Non-printable, so loop through our array of structs  */
		int n = 0;
	
		do {
			if ( keys[n].code == ch )
			return keys[n].name;
			n++;
		} while ( keys[n].code != -1 );

		return keys[n].name;

    }    
    
    return NULL;        /*  We shouldn't get here  */
}

