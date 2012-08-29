/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *		Werner Freytag <freytag@gmx.de>
 * Authors:
 *		Stephan Aßmus <superstippi@gmx.de>
 *		John Scipione <jscipione@gmail.com>
 */


#include "ColorPickerView.h"

#include <stdio.h>
#include <stdlib.h>

#include <Application.h>
#include <Bitmap.h>
#include <Beep.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <LayoutBuilder.h>
#include <Message.h>
#include <RadioButton.h>
#include <Resources.h>
#include <Screen.h>
#include <Size.h>
#include <TextControl.h>
#include <Window.h>

#include "BackgroundView.h"
#include "ColorField.h"
#include "ColorsApplication.h"
#include "ColorPreview.h"
#include "ColorSlider.h"
#include "ColorsWindow.h"
#include "convert_rgb_hsv.h"
#include "EyeDropper.h"

#define round(x) (int)(x+.5)
#define hexdec(str, offset) (int)(((str[offset]<60?str[offset]-48:(str[offset]|32)-87)<<4)|(str[offset+1]<60?str[offset+1]-48:(str[offset+1]|32)-87))


ColorPickerView::ColorPickerView()
	:
	BView("color picker view", B_WILL_DRAW | B_PULSE_NEEDED),
	fColorMode(S_SELECTED),
	fHue(0.0),
	fSat(1.0),
	fVal(1.0),
	fRed(1.0),
	fGreen(0.0),
	fBlue(0.0),
	fRequiresUpdate(false)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	long int int_color;
	if (settings->FindInt32("selected_color", &int_color) == B_OK) {
		fRed = (float)(int_color >> 16) / 255;
		fGreen = (float)((int_color >> 8) & 255) / 255;
		fBlue = (float)(int_color & 255) / 255;

		RGB_to_HSV(fRed, fGreen, fBlue, fHue, fSat, fVal);
	}

	fColorField = new ColorField(fColorMode, fSat);
	fColorSlider = new ColorSlider(fColorMode, fHue, fVal);
	fColorPreview = new ColorPreview();
	fEyeDropper = new EyeDropper();

	const char *title[] = { "H", "S", "V", "R", "G", "B" };

	for (int32 i = 0; i < 6; ++i) {
		fRadioButton[i] = new BRadioButton(NULL, title[i],
			new BMessage(MSG_RADIOBUTTON + i));
		fRadioButton[i]->SetExplicitMinSize(BSize(32.0, 19.0));

		fTextControl[i] = new BTextControl(NULL, NULL, NULL,
			new BMessage(MSG_TEXTCONTROL + i));
	}

	int32 selectedMode;
	if (settings->FindInt32("selected_mode", &selectedMode) < B_OK) {
		// default to saturation mode
		selectedMode = 1;
	}
	fRadioButton[selectedMode]->SetValue(B_CONTROL_ON);
	fColorMode = (color_mode)(1 << selectedMode);

	fHexTextControl = new BTextControl(NULL, "#", NULL,
		new BMessage(MSG_HEXTEXTCONTROL));

	BLayoutBuilder::Group<>(this)
		.AddGroup(B_HORIZONTAL, B_USE_SMALL_SPACING)
			.Add(fColorField)
			.Add(fColorSlider)
			.AddGroup(B_VERTICAL, B_USE_SMALL_SPACING)
				.Add(fColorPreview)
				.Add(fEyeDropper)
				.AddGrid(1, 1)
					.Add(fRadioButton[0], 0, 0)
					.Add(fTextControl[0], 1, 0)
					.Add(fRadioButton[1], 0, 1)
					.Add(fTextControl[1], 1, 1)
					.Add(fRadioButton[2], 0, 2)
					.Add(fTextControl[2], 1, 2)
					.Add(fRadioButton[3], 0, 3)
					.Add(fTextControl[3], 1, 3)
					.Add(fRadioButton[4], 0, 4)
					.Add(fTextControl[4], 1, 4)
					.Add(fRadioButton[5], 0, 5)
					.Add(fTextControl[5], 1, 5)
					.End()
				.Add(fHexTextControl)
				.AddGlue()
				.End()
			.SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
			.End();
}


ColorPickerView::~ColorPickerView()
{
}


void
ColorPickerView::AttachedToWindow()
{
	BView::AttachedToWindow();

	rgb_color selected_color = {
		(int)(fRed * 255),
		(int)(fGreen * 255),
		(int)(fBlue * 255), 255
	};

	fColorField->SetMarkerToColor(selected_color);
	fColorField->SetTarget(this);

	fColorSlider->SetMarkerToColor(selected_color);
	fColorSlider->SetTarget(this);

	fColorPreview->SetNewColor(selected_color);
	fColorPreview->SetTarget(this);

	fEyeDropper->SetTarget(this);

	for (int32 i = 0; i < 6; ++i) {
		fRadioButton[i]->SetFontSize(9.0);
		fRadioButton[i]->SetTarget(this);

		fTextControl[i]->SetDivider(12.0);
		fTextControl[i]->SetTarget(this);

		// Only permit (max 3) decimal inputs
		BTextView *textView = fTextControl[i]->TextView();
		textView->SetMaxBytes(3);
		for (int32 j = 32; j < 255; ++j) {
			if (j < '0' || j > '9')
				textView->DisallowChar(j);
		}
	}

	fHexTextControl->SetDivider(12.0);
	fHexTextControl->SetTarget(this);

	// Only permit (max 6) hexidecimal inputs
	BTextView *hexTextView = fHexTextControl->TextView();
	hexTextView->SetMaxBytes(6);
	for (int32 j = 32; j < 255; ++j) {
		if (!((j >= '0' && j <= '9') || (j >= 'a' && j <= 'f')
			|| (j >= 'A' && j <= 'F'))) {
			hexTextView->DisallowChar(j);
		}
	}

	SetColorMode(S_SELECTED);
	_UpdateTextControls();
}


void
ColorPickerView::Draw(BRect updateRect)
{
	// raised border
	BRect rect(Bounds());

	if (updateRect.Intersects(rect)) {
		rgb_color light = tint_color(LowColor(), B_LIGHTEN_MAX_TINT);
		rgb_color shadow = tint_color(LowColor(), B_DARKEN_2_TINT);

		BeginLineArray(4);
			AddLine(BPoint(rect.left, rect.bottom),
					BPoint(rect.left, rect.top), light);
			AddLine(BPoint(rect.left + 1.0, rect.top),
					BPoint(rect.right, rect.top), light);
			AddLine(BPoint(rect.right, rect.top + 1.0),
					BPoint(rect.right, rect.bottom), shadow);
			AddLine(BPoint(rect.right - 1.0, rect.bottom),
					BPoint(rect.left + 1.0, rect.bottom), shadow);
		EndLineArray();

		// exclude border from update rect
		rect.InsetBy(1.0, 1.0);
		updateRect = rect & updateRect;
	}
}


void
ColorPickerView::MessageReceived(BMessage *message)
{
	switch (message->what) {
		case MSG_COLOR_FIELD:
		{
			float value1;
			float value2;
			value1 = message->FindFloat("value");
			value2 = message->FindFloat("value", 1);
			_UpdateColor(-1, value1, value2);
			fRequiresUpdate = true;
			break;
		}

		case MSG_COLOR_SLIDER:
		{
			float value;
			message->FindFloat("value", &value);
			_UpdateColor(value, -1, -1);
			fRequiresUpdate = true;
			break;
		}

		case MSG_COLOR_PREVIEW:
		{
			rgb_color* color;
			ssize_t numBytes;
			if (message->FindData("selected_color", B_RGB_COLOR_TYPE,
					(const void **)&color, &numBytes) == B_OK) {
				color->alpha = 255;
				SetColor(*color);
			}
			break;
		}

		case MSG_EYEDROPPER:
			_GrabColor();
			break;

		case MSG_RADIOBUTTON:
			SetColorMode(H_SELECTED);
			break;

		case MSG_RADIOBUTTON + 1:
			SetColorMode(S_SELECTED);
			break;

		case MSG_RADIOBUTTON + 2:
			SetColorMode(V_SELECTED);
			break;

		case MSG_RADIOBUTTON + 3:
			SetColorMode(R_SELECTED);
			break;

		case MSG_RADIOBUTTON + 4:
			SetColorMode(G_SELECTED);
			break;

		case MSG_RADIOBUTTON + 5:
			SetColorMode(B_SELECTED);
			break;

		case MSG_TEXTCONTROL:
		case MSG_TEXTCONTROL + 1:
		case MSG_TEXTCONTROL + 2:
		case MSG_TEXTCONTROL + 3:
		case MSG_TEXTCONTROL + 4:
		case MSG_TEXTCONTROL + 5:
		{
			int nr = message->what - MSG_TEXTCONTROL;
			int value = atoi(fTextControl[nr]->Text());
			char string[4];

			switch (nr) {
				case 0:
				{
					value %= 360;
					sprintf(string, "%d", value);
					fHue = (float)value / 60;
					break;
				}

				case 1:
				{
					value = min_c(value, 100);
					sprintf(string, "%d", value);
					fSat = (float)value / 100;
					break;
				}

				case 2:
				{
					value = min_c(value, 100);
					sprintf(string, "%d", value);
					fVal = (float)value / 100;
					break;
				}

				case 3:
				{
					value = min_c(value, 255);
					sprintf(string, "%d", value);
					fRed = (float)value / 255;
					break;
				}

				case 4:
				{
					value = min_c(value, 255);
					sprintf(string, "%d", value);
					fGreen = (float)value / 255;
					break;
				}

				case 5:
				{
					value = min_c(value, 255);
					sprintf(string, "%d", value);
					fBlue = (float)value / 255;
					break;
				}
			}

			if (nr < 3) {
				// hsv-mode
				HSV_to_RGB(fHue, fSat, fVal, fRed, fGreen, fBlue);
			}

			rgb_color color = { round(fRed * 255), round(fGreen * 255),
				round(fBlue * 255), 255 };

			SetColor(color);
			break;
		}

		case MSG_HEXTEXTCONTROL:
		{
			if (fHexTextControl->TextView()->TextLength() == 6) {
				const char *string = fHexTextControl->TextView()->Text();
				rgb_color color = { hexdec(string, 0), hexdec(string, 2),
					hexdec(string, 4), 255 };
				SetColor(color);
			}
			break;
		}

		default:
			BView::MessageReceived(message);
	}
}


void
ColorPickerView::MouseDown(BPoint where)
{
	Window()->Activate();

	if (Window()->CurrentMessage()->FindInt32("buttons")
			== B_SECONDARY_MOUSE_BUTTON) {
		BPoint where;
		Window()->CurrentMessage()->FindPoint("where", &where);
		ConvertToScreen(&where);

		BMessage message(MSG_POPUP_MENU);
		message.AddPoint("where", where);
		Window()->PostMessage(&message);
	} else {
		fMouseDown = true;
		fMouseOffset = where;

		SetMouseEventMask(B_POINTER_EVENTS, B_NO_POINTER_HISTORY
			| B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);

		BView::MouseDown(where);
	}
}


void
ColorPickerView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	if (fMouseDown) {
		BPoint win_pos = Window()->Frame().LeftTop();
		Window()->MoveTo(win_pos.x + where.x - fMouseOffset.x,
			win_pos.y + where.y - fMouseOffset.y );
	} else
		BView::MouseMoved(where, code, message);
}


void
ColorPickerView::MouseUp(BPoint where)
{
	fMouseDown = false;
	BView::MouseUp(where);
}


void
ColorPickerView::Pulse()
{
	if (fRequiresUpdate)
		_UpdateTextControls();
}


// #pragma mark -


rgb_color
ColorPickerView::Color()
{
	if (fColorMode & (R_SELECTED | G_SELECTED | B_SELECTED))
		RGB_to_HSV(fRed, fGreen, fBlue, fHue, fSat, fVal);
	else
		HSV_to_RGB(fRed, fGreen, fBlue, fHue, fSat, fVal);

	rgb_color color;
	color.red = (uint8)round(fRed * 255.0);
	color.green = (uint8)round(fGreen * 255.0);
	color.blue = (uint8)round(fBlue * 255.0);
	color.alpha = 255;

	return color;
}


void
ColorPickerView::SetColor(rgb_color base)
{
	fRed = (float)base.red / 255;
	fGreen = (float)base.green / 255;
	fBlue = (float)base.blue / 255;
	RGB_to_HSV(fRed, fGreen, fBlue, fHue, fSat, fVal);

	fColorSlider->SetModeAndValues(fColorMode, *fPointer1, *fPointer2);
	fColorSlider->SetMarkerToColor(base);

	fColorField->SetModeAndValue(fColorMode, *fPointer0);
	fColorField->SetMarkerToColor(base);

	fColorPreview->SetColor(base);

	fRequiresUpdate = true;
}


void
ColorPickerView::SetColorMode(color_mode mode)
{
	fColorMode = mode;

	switch (mode) {
		case R_SELECTED:
			fPointer0 = &fRed;
			fPointer1 = &fGreen;
			fPointer2 = &fBlue;
			break;

		case G_SELECTED:
			fPointer0 = &fGreen;
			fPointer1 = &fRed;
			fPointer2 = &fBlue;
			break;

		case B_SELECTED:
			fPointer0 = &fBlue;
			fPointer1 = &fRed;
			fPointer2 = &fGreen;
			break;

		case H_SELECTED:
			fPointer0 = &fHue;
			fPointer1 = &fSat;
			fPointer2 = &fVal;
			break;

		case S_SELECTED:
			fPointer0 = &fSat;
			fPointer1 = &fHue;
			fPointer2 = &fVal;
			break;

		case V_SELECTED:
			fPointer0 = &fVal;
			fPointer1 = &fHue;
			fPointer2 = &fSat;
			break;
	}

	fColorSlider->SetModeAndValues(fColorMode, *fPointer1, *fPointer2);
	fColorField->SetModeAndValue(fColorMode, *fPointer0);
}


void
ColorPickerView::SaveSettings()
{
	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	settings->RemoveName("selected_color");
	int32 int_color = ((int)(fRed * 255) << 16)
		+ ((int)(fGreen * 255) << 8)
		+ ((int)(fBlue * 255));
	settings->AddInt32("selected_color", int_color);

	settings->RemoveName("selected_mode");
	for (int32 i = 0; i < 6; ++i) {
		if (fRadioButton[i]->Value() == B_CONTROL_ON) {
			settings->AddInt32("selected_mode", i);
			break;
		}
	}
}


// #pragma mark -


void
ColorPickerView::_GrabColor()
{
	BScreen screen(Window());
	BWindow *win = new BWindow(screen.Frame(), "", B_NO_BORDER_WINDOW_LOOK,
		B_FLOATING_ALL_WINDOW_FEEL, 0);
	BView *view = new BackgroundView(screen.Frame().OffsetToCopy(0.0, 0.0));
	win->AddChild(view);
	view->MakeFocus();

	win->Show();
}


void
ColorPickerView::_UpdateColor(float value, float value1, float value2)
{
	if (value != -1) {
		fColorField->SetFixedValue(value);
		*fPointer0 = value;
	} else if (value1 != -1 && value2 != -1) {
		fColorSlider->SetOtherValues(value1, value2);
		*fPointer1 = value1; *fPointer2 = value2;
	}

	if (fColorMode & (R_SELECTED | G_SELECTED | B_SELECTED))
		RGB_to_HSV(fRed, fGreen, fBlue, fHue, fSat, fVal);
	else
		HSV_to_RGB(fHue, fSat, fVal, fRed, fGreen, fBlue);

	rgb_color base = { (int)(fRed * 255), (int)(fGreen * 255),
		(int)(fBlue * 255), 255 };
	fColorPreview->SetColor(base);
}


void
ColorPickerView::_UpdateTextControls()
{
	Window()->DisableUpdates();

	char string[5];

	sprintf(string, "%d", round(fHue * 60));
	fTextControl[0]->TextView()->SetText(string);

	sprintf(string, "%d", round(fSat * 100));
	fTextControl[1]->TextView()->SetText(string);

	sprintf(string, "%d", round(fVal * 100));
	fTextControl[2]->TextView()->SetText(string);

	sprintf(string, "%d", round(fRed * 255));
	fTextControl[3]->TextView()->SetText(string);
	
	sprintf(string, "%d", round(fGreen * 255));
	fTextControl[4]->TextView()->SetText(string);

	sprintf(string, "%d", round(fBlue * 255));
	fTextControl[5]->TextView()->SetText(string);

	sprintf(string, "%.6X", (round(fRed * 255) << 16) | (round(fGreen * 255) << 8)
		| round(fBlue * 255));
	fHexTextControl->TextView()->SetText(string);

	Window()->EnableUpdates();

	fRequiresUpdate = false;
}
