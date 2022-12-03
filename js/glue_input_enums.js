
const MouseBtn_None    = 0;
const MouseBtn_Left    = 1;
const MouseBtn_Right   = 2;
const MouseBtn_Middle  = 3;
const MouseBtn_NumBtns = 4;

const MouseBtn_Dictionary =
{
	0: MouseBtn_Left,
	1: MouseBtn_Middle,
	2: MouseBtn_Right,
};

function MouseDownEventNumToBtnEnum(btnNum)
{
	if (MouseBtn_Dictionary[btnNum] !== undefined)
	{
		return MouseBtn_Dictionary[btnNum];
	}
	else
	{
		console.log("Unknown btnNum in mouse event: \"" + btnNum + "\"");
		return MouseBtn_NumBtns;
	}
}

const Key_None         = 0;

const Key_A            = 1;
const Key_B            = 2;
const Key_C            = 3;
const Key_D            = 4;
const Key_E            = 5;
const Key_F            = 6;
const Key_G            = 7;
const Key_H            = 8;
const Key_I            = 9;
const Key_J            = 10;
const Key_K            = 11;
const Key_L            = 12;
const Key_M            = 13;
const Key_N            = 14;
const Key_O            = 15;
const Key_P            = 16;
const Key_Q            = 17;
const Key_R            = 18;
const Key_S            = 19;
const Key_T            = 20;
const Key_U            = 21;
const Key_V            = 22;
const Key_W            = 23;
const Key_X            = 24;
const Key_Y            = 25;
const Key_Z            = 26;

const Key_0            = 27;
const Key_1            = 28;
const Key_2            = 29;
const Key_3            = 30;
const Key_4            = 31;
const Key_5            = 32;
const Key_6            = 33;
const Key_7            = 34;
const Key_8            = 35;
const Key_9            = 36;

const Key_Num0         = 37;
const Key_Num1         = 38;
const Key_Num2         = 39;
const Key_Num3         = 40;
const Key_Num4         = 41;
const Key_Num5         = 42;
const Key_Num6         = 43;
const Key_Num7         = 44;
const Key_Num8         = 45;
const Key_Num9         = 46;

const Key_NumPeriod    = 47;
const Key_NumDivide    = 48;
const Key_NumMultiply  = 49;
const Key_NumSubtract  = 50;
const Key_NumAdd       = 51;

const Key_F1           = 52;
const Key_F2           = 53;
const Key_F3           = 54;
const Key_F4           = 55;
const Key_F5           = 56;
const Key_F6           = 57;
const Key_F7           = 58;
const Key_F8           = 59;
const Key_F9           = 60;
const Key_F10          = 61;
const Key_F11          = 62;
const Key_F12          = 63;

const Key_Enter        = 64;
const Key_Backspace    = 65;
const Key_Escape       = 66;
const Key_Insert       = 67;
const Key_Delete       = 68;
const Key_Home         = 69;
const Key_End          = 70;
const Key_PageUp       = 71;
const Key_PageDown     = 72;
const Key_Tab          = 73;
const Key_CapsLock     = 74;
const Key_NumLock      = 75;

const Key_Control      = 76;
const Key_Alt          = 77;
const Key_Shift        = 78;

const Key_Right        = 79;
const Key_Left         = 80;
const Key_Up           = 81;
const Key_Down         = 82;

const Key_Plus         = 83;
const Key_Minus        = 84;
const Key_Pipe         = 85;
const Key_OpenBracket  = 86;
const Key_CloseBracket = 87;
const Key_Colon        = 88;
const Key_Quote        = 89;
const Key_Comma        = 90;
const Key_Period       = 91;
const Key_QuestionMark = 92;
const Key_Tilde        = 93;
const Key_Space        = 94;

const Key_NumKeys      = 95;

const Key_Dictionary =
{
	"Enter":          Key_Enter,
	"NumpadEnter":    Key_Enter,
	"Space":          Key_Space,
	"Backspace":      Key_Backspace,
	"Tab":            Key_Tab,
	"Escape":         Key_Escape,
	"ShiftLeft":      Key_Shift,
	"ShiftRight":     Key_Shift,
	"ControlLeft":    Key_Control,
	"ControlRight":   Key_Control,
	"AltLeft":        Key_Alt,
	"AltRight":       Key_Alt,
	"Backquote":      Key_Tilde,
	"Insert":         Key_Insert,
	"Delete":         Key_Delete,
	"Home":           Key_Home,
	"End":            Key_End,
	"PageUp":         Key_PageUp,
	"PageDown":       Key_PageDown,
	"KeyA":           Key_A,
	"KeyB":           Key_B,
	"KeyC":           Key_C,
	"KeyD":           Key_D,
	"KeyE":           Key_E,
	"KeyF":           Key_F,
	"KeyG":           Key_G,
	"KeyH":           Key_H,
	"KeyI":           Key_I,
	"KeyJ":           Key_J,
	"KeyK":           Key_K,
	"KeyL":           Key_L,
	"KeyM":           Key_M,
	"KeyN":           Key_N,
	"KeyO":           Key_O,
	"KeyP":           Key_P,
	"KeyQ":           Key_Q,
	"KeyR":           Key_R,
	"KeyS":           Key_S,
	"KeyT":           Key_T,
	"KeyU":           Key_U,
	"KeyV":           Key_V,
	"KeyW":           Key_W,
	"KeyX":           Key_X,
	"KeyY":           Key_Y,
	"KeyZ":           Key_Z,
	"Backslash":      Key_Pipe,
	"BracketLeft":    Key_OpenBracket,
	"BracketRight":   Key_CloseBracket,
	"Semicolon":      Key_Colon,
	"Quote":          Key_Quote,
	"Slash":          Key_QuestionMark,
	"Period":         Key_Period,
	"Comma":          Key_Comma,
	"Minus":          Key_Minus,
	"Equal":          Key_Plus,
	"Digit1":         Key_1,
	"Digit2":         Key_2,
	"Digit3":         Key_3,
	"Digit4":         Key_4,
	"Digit5":         Key_5,
	"Digit6":         Key_6,
	"Digit7":         Key_7,
	"Digit8":         Key_8,
	"Digit9":         Key_9,
	"Digit0":         Key_0,
	"Numpad0":        Key_Num0,
	"Numpad1":        Key_Num1,
	"Numpad2":        Key_Num2,
	"Numpad3":        Key_Num3,
	"Numpad4":        Key_Num4,
	"Numpad5":        Key_Num5,
	"Numpad6":        Key_Num6,
	"Numpad7":        Key_Num7,
	"Numpad8":        Key_Num8,
	"Numpad9":        Key_Num9,
	"NumpadDecimal":  Key_NumPeriod,
	"NumpadAdd":      Key_NumAdd,
	"NumpadSubtract": Key_NumSubtract,
	"NumpadMultiply": Key_NumMultiply,
	"NumpadDivide":   Key_NumDivide,
	"F1":             Key_F1,
	"F2":             Key_F2,
	"F3":             Key_F3,
	"F4":             Key_F4,
	"F5":             Key_F5,
	"F6":             Key_F6,
	"F7":             Key_F7,
	"F8":             Key_F8,
	"F9":             Key_F9,
	"F10":            Key_F10,
	"F11":            Key_F11,
	"F12":            Key_F12,
	"NumLock":        Key_NumLock,
	// "ScrollLock":     Key_ScrollLock, //no enum value on our side
	// "Pause":          Key_Pause,      //no enum value on our side
};

function KeyDownEventStrToKeyEnum(keyStr)
{
	if (Key_Dictionary[keyStr] !== undefined)
	{
		return Key_Dictionary[keyStr];
	}
	else
	{
		console.log("Unknown KeyDownEventStr: \"" + keyStr + "\"");
		return Key_NumKeys;
	}
}
