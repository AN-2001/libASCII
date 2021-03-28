#ifndef ASCII_JOYSTICK_H
#define ASCII_JOYSTICK_H
//NOTE1: this module is hard coded for the logitech f310 controller
//		 no attempts have been made to make it more portable
//
//NOTE2: the code as it stands can only handle openning and handling ONE joystick

typedef struct ascii_joystick_t *ASCIIJoyStick;

typedef enum{
	ASCII_BTN_INACTIVE = 0,
	ASCII_BTN_PRESSED,
	ASCII_BTN_RELEASED
} ASCIIBtnState;

typedef double ASCIIAxisState;

//BUTTON ENUM
typedef enum{
	ASCII_BTN_A = 0,
	ASCII_BTN_B,
	ASCII_BTN_X = 3,
	ASCII_BTN_Y,
	ASCII_BTN_LB = 6,
	ASCII_BTN_RB,
	ASCII_BTN_SELECT = 10,
	ASCII_BTN_START,
	ASCII_BTN_MODE,
	ASCII_BTN_LTHUMB,
	ASCII_BTN_RTHUMB,
} ASCIIBtn;


//AXIS ENUM
typedef enum{
	ASCII_AXIS_LX = 0,
	ASCII_AXIS_LY,
	ASCII_AXIS_LT,
	ASCII_AXIS_RX,
	ASCII_AXIS_RY,
	ASCII_AXIS_RT,
	ASCII_DPAD_X = 10,
	ASCII_DPAD_Y,
} ASCIIAxis;

//create a joystick instance
ASCIIJoyStick ASCIIJoyStickCreate();

//query for a button
ASCIIBtnState ASCIIJoyStickGetButton(ASCIIJoyStick joystick, ASCIIBtn button);

//query for an axis
ASCIIAxisState ASCIIJoyStickGetAxis(ASCIIJoyStick joystick, ASCIIAxis axis);

//start the joystick
void ASCIIJoyStickStart(ASCIIJoyStick joystick);
//stop the joystick
void ASCIIJoyStickStop(ASCIIJoyStick joystick);
//free a joystick's resources
void ASCIIJoyStickDestroy(ASCIIJoyStick joystick);
#endif //ASCII_INPUT_H
