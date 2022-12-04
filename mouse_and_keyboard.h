/*
File:   mouse_and_keyboard.h
Author: Taylor Robbins
Date:   12\02\2022
*/

#ifndef _MOUSE_AND_KEYBOARD_H
#define _MOUSE_AND_KEYBOARD_H

#include "vectors.h"

// +--------------------------------------------------------------+
// |                         Enumerations                         |
// +--------------------------------------------------------------+
enum MouseBtn_t
{
	MouseBtn_None = 0,
	MouseBtn_Left,
	MouseBtn_Right,
	MouseBtn_Middle,
	MouseBtn_NumBtns,
};
const char* GetMouseBtnStr(MouseBtn_t enumValue)
{
	switch (enumValue)
	{
		case MouseBtn_None:   return "None";
		case MouseBtn_Left:   return "Left";
		case MouseBtn_Right:  return "Right";
		case MouseBtn_Middle: return "Middle";
		default: return "Unknown";
	}
}

enum Key_t
{
	Key_None = 0,
	
	Key_A,
	Key_B,
	Key_C,
	Key_D,
	Key_E,
	Key_F,
	Key_G,
	Key_H,
	Key_I,
	Key_J,
	Key_K,
	Key_L,
	Key_M,
	Key_N,
	Key_O,
	Key_P,
	Key_Q,
	Key_R,
	Key_S,
	Key_T,
	Key_U,
	Key_V,
	Key_W,
	Key_X,
	Key_Y,
	Key_Z,
	
	Key_0,
	Key_1,
	Key_2,
	Key_3,
	Key_4,
	Key_5,
	Key_6,
	Key_7,
	Key_8,
	Key_9,
	
	Key_Num0,
	Key_Num1,
	Key_Num2,
	Key_Num3,
	Key_Num4,
	Key_Num5,
	Key_Num6,
	Key_Num7,
	Key_Num8,
	Key_Num9,
	
	Key_NumPeriod,
	Key_NumDivide,
	Key_NumMultiply,
	Key_NumSubtract,
	Key_NumAdd,
	
	Key_F1,
	Key_F2,
	Key_F3,
	Key_F4,
	Key_F5,
	Key_F6,
	Key_F7,
	Key_F8,
	Key_F9,
	Key_F10,
	Key_F11,
	Key_F12,
	
	Key_Enter,
	Key_Backspace,
	Key_Escape,
	Key_Insert,
	Key_Delete,
	Key_Home,
	Key_End,
	Key_PageUp,
	Key_PageDown,
	Key_Tab,
	Key_CapsLock,
	Key_NumLock,
	
	Key_Control,
	Key_Alt,
	Key_Shift,
	
	Key_Right,
	Key_Left,
	Key_Up,
	Key_Down,
	
	Key_Plus,
	Key_Minus,
	Key_Pipe,
	Key_OpenBracket,
	Key_CloseBracket,
	Key_Colon,
	Key_Quote,
	Key_Comma,
	Key_Period,
	Key_QuestionMark,
	Key_Tilde,
	Key_Space,
	
	Key_NumKeys,
};
const char* GetKeyStr(Key_t key)
{
	switch (key)
	{
		case Key_None:         return "None";
		case Key_A:            return "A";
		case Key_B:            return "B";
		case Key_C:            return "C";
		case Key_D:            return "D";
		case Key_E:            return "E";
		case Key_F:            return "F";
		case Key_G:            return "G";
		case Key_H:            return "H";
		case Key_I:            return "I";
		case Key_J:            return "J";
		case Key_K:            return "K";
		case Key_L:            return "L";
		case Key_M:            return "M";
		case Key_N:            return "N";
		case Key_O:            return "O";
		case Key_P:            return "P";
		case Key_Q:            return "Q";
		case Key_R:            return "R";
		case Key_S:            return "S";
		case Key_T:            return "T";
		case Key_U:            return "U";
		case Key_V:            return "V";
		case Key_W:            return "W";
		case Key_X:            return "X";
		case Key_Y:            return "Y";
		case Key_Z:            return "Z";
		case Key_0:            return "0";
		case Key_1:            return "1";
		case Key_2:            return "2";
		case Key_3:            return "3";
		case Key_4:            return "4";
		case Key_5:            return "5";
		case Key_6:            return "6";
		case Key_7:            return "7";
		case Key_8:            return "8";
		case Key_9:            return "9";
		case Key_Num0:         return "Num0";
		case Key_Num1:         return "Num1";
		case Key_Num2:         return "Num2";
		case Key_Num3:         return "Num3";
		case Key_Num4:         return "Num4";
		case Key_Num5:         return "Num5";
		case Key_Num6:         return "Num6";
		case Key_Num7:         return "Num7";
		case Key_Num8:         return "Num8";
		case Key_Num9:         return "Num9";
		case Key_NumPeriod:    return "NumPeriod";
		case Key_NumDivide:    return "NumDivide";
		case Key_NumMultiply:  return "NumMultiply";
		case Key_NumSubtract:  return "NumSubtract";
		case Key_NumAdd:       return "NumAdd";
		case Key_F1:           return "F1";
		case Key_F2:           return "F2";
		case Key_F3:           return "F3";
		case Key_F4:           return "F4";
		case Key_F5:           return "F5";
		case Key_F6:           return "F6";
		case Key_F7:           return "F7";
		case Key_F8:           return "F8";
		case Key_F9:           return "F9";
		case Key_F10:          return "F10";
		case Key_F11:          return "F11";
		case Key_F12:          return "F12";
		case Key_Enter:        return "Enter";
		case Key_Backspace:    return "Backspace";
		case Key_Escape:       return "Escape";
		case Key_Insert:       return "Insert";
		case Key_Delete:       return "Delete";
		case Key_Home:         return "Home";
		case Key_End:          return "End";
		case Key_PageUp:       return "PageUp";
		case Key_PageDown:     return "PageDown";
		case Key_Tab:          return "Tab";
		case Key_CapsLock:     return "CapsLock";
		case Key_NumLock:      return "NumLock";
		case Key_Control:      return "Control";
		case Key_Alt:          return "Alt";
		case Key_Shift:        return "Shift";
		case Key_Right:        return "Right";
		case Key_Left:         return "Left";
		case Key_Up:           return "Up";
		case Key_Down:         return "Down";
		case Key_Plus:         return "Plus";
		case Key_Minus:        return "Minus";
		case Key_Pipe:         return "Pipe";
		case Key_OpenBracket:  return "OpenBracket";
		case Key_CloseBracket: return "CloseBracket";
		case Key_Colon:        return "Colon";
		case Key_Quote:        return "Quote";
		case Key_Comma:        return "Comma";
		case Key_Period:       return "Period";
		case Key_QuestionMark: return "QuestionMark";
		case Key_Tilde:        return "Tilde";
		case Key_Space:        return "Space";
		default: return "Unknown";
	}
}

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
struct MouseBtnState_t
{
	MouseBtn_t btn;
	bool isDown;
	bool wasPressed;
	bool wasReleased;
};
struct Mouse_t
{
	v2 position;
	MouseBtnState_t buttons[MouseBtn_NumBtns];
};

struct KeyState_t
{
	Key_t key;
	bool isDown;
	bool wasPressed;
	bool wasReleased;
};
struct Keyboard_t
{
	KeyState_t keys[Key_NumKeys];
};

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
extern Mouse_t* mouse;
extern Keyboard_t* keyboard;

// +--------------------------------------------------------------+
// |                          Functions                           |
// +--------------------------------------------------------------+
void InitMouse(Mouse_t* mousePntr);
void InitKeyboard(Keyboard_t* keyboardPntr);

void MouseHandleBtnStateChange(Mouse_t* mouse, MouseBtn_t btn, bool isDown);
void KeyboardHandleKeyStateChange(Keyboard_t* keyboard, Key_t key, bool isDown);
void ResetMouseState(Mouse_t* mouse);
void ResetKeyboardState(Keyboard_t* keyboard);

bool MouseDown(MouseBtn_t btn);
bool MousePressed(MouseBtn_t btn);
bool MouseReleased(MouseBtn_t btn);

bool KeyDown(Key_t key);
bool KeyPressed(Key_t key);
bool KeyReleased(Key_t key);

#endif //  _MOUSE_AND_KEYBOARD_H
