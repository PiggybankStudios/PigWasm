/*
File:   mouse_and_keyboard.cpp
Author: Taylor Robbins
Date:   12\02\2022
Description: 
	** Holds functions that handle routing and converting raw keyboard and mouse
	** events that are passed to us from the javascript front-end
*/

#include "common.h"
#include "mouse_and_keyboard.h"

#include "memory.h"
#include "intrinsics.h"

// +--------------------------------------------------------------+
// |                           Globals                            |
// +--------------------------------------------------------------+
Mouse_t* mouse = nullptr;
Keyboard_t* keyboard = nullptr;

// +--------------------------------------------------------------+
// |                          Initialize                          |
// +--------------------------------------------------------------+
void InitMouse(Mouse_t* mousePntr)
{
	NotNull(mousePntr);
	mouse = mousePntr;
	ClearPointer(mouse);
	for (u32 bIndex = 0; bIndex < MouseBtn_NumBtns; bIndex++)
	{
		mouse->buttons[bIndex].btn = (MouseBtn_t)bIndex;
	}
}
void InitKeyboard(Keyboard_t* keyboardPntr)
{
	NotNull(keyboardPntr);
	keyboard = keyboardPntr;
	ClearPointer(keyboard);
	for (u32 keyIndex = 0; keyIndex < Key_NumKeys; keyIndex++)
	{
		keyboard->keys[keyIndex].key = (Key_t)keyIndex;
	}
}

// +--------------------------------------------------------------+
// |                      HandleStateChange                       |
// +--------------------------------------------------------------+
void MouseHandleBtnStateChange(Mouse_t* mouse, MouseBtn_t btn, bool isDown)
{
	NotNull(mouse);
	if (btn >= MouseBtn_NumBtns) { return; }
	MouseBtnState_t* state = &mouse->buttons[btn];
	if (state->isDown != isDown)
	{
		state->isDown = isDown;
		if (isDown) { state->wasPressed = true; }
		else { state->wasReleased = true; }
	}
}

void KeyboardHandleKeyStateChange(Keyboard_t* keyboard, Key_t key, bool isDown)
{
	NotNull(keyboard);
	if (key >= Key_NumKeys) { return; }
	KeyState_t* state = &keyboard->keys[key];
	if (state->isDown != isDown)
	{
		state->isDown = isDown;
		if (isDown) { state->wasPressed = true; }
		else { state->wasReleased = true; }
	}
}

void ResetMouseState(Mouse_t* mouse)
{
	NotNull(mouse);
	for (u32 bIndex = 0; bIndex < MouseBtn_NumBtns; bIndex++)
	{
		mouse->buttons[bIndex].wasPressed = false;
		mouse->buttons[bIndex].wasReleased = false;
	}
}

void ResetKeyboardState(Keyboard_t* keyboard)
{
	NotNull(keyboard);
	for (u32 keyIndex = 0; keyIndex < Key_NumKeys; keyIndex++)
	{
		keyboard->keys[keyIndex].wasPressed = false;
		keyboard->keys[keyIndex].wasReleased = false;
	}
}

// +--------------------------------------------------------------+
// |                             API                              |
// +--------------------------------------------------------------+
bool MouseDown(MouseBtn_t btn)
{
	if (btn >= MouseBtn_NumBtns) { return false; }
	return mouse->buttons[btn].isDown;
}
bool MousePressed(MouseBtn_t btn)
{
	if (btn >= MouseBtn_NumBtns) { return false; }
	return mouse->buttons[btn].wasPressed;
}
bool MouseReleased(MouseBtn_t btn)
{
	if (btn >= MouseBtn_NumBtns) { return false; }
	return mouse->buttons[btn].wasReleased;
}

bool KeyDown(Key_t key)
{
	if (key >= Key_NumKeys) { return false; }
	return keyboard->keys[key].isDown;
}
bool KeyPressed(Key_t key)
{
	if (key >= Key_NumKeys) { return false; }
	return keyboard->keys[key].wasPressed;
}
bool KeyReleased(Key_t key)
{
	if (key >= Key_NumKeys) { return false; }
	return keyboard->keys[key].wasReleased;
}

