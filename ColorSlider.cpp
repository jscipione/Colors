/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *		Werner Freytag <freytag@gmx.de>
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "ColorSlider.h"

#include <iostream>
#include <math.h>

#include <Bitmap.h>
#include <OS.h>
#include <Window.h>

#include "convert_rgb_hsv.h"
#include "color_mode.h"

#define COLOR_SLIDER_WIDTH 35.0
#define COLOR_SLIDER_HEIGHT 259.0
#define COLOR_SLIDER_RECT BRect(0.0, 0.0, COLOR_SLIDER_WIDTH, \
	COLOR_SLIDER_HEIGHT)
#define round(x) (int)(x+0.5)


ColorSlider::ColorSlider(color_mode mode, float value1, float value2)
	:
	BControl("color slider", "", new BMessage(MSG_COLOR_SLIDER), B_WILL_DRAW),
	fColorMode(mode),
	fFixedValue1(value1),
	fFixedValue2(value2),
	fMouseDown(false),
	fUpdateThread(0),
	fUpdatePort(0)
{
	SetExplicitMinSize(BSize(COLOR_SLIDER_WIDTH, COLOR_SLIDER_HEIGHT));
	SetExplicitMaxSize(BSize(COLOR_SLIDER_WIDTH, COLOR_SLIDER_HEIGHT));

	fBgBitmap = new BBitmap(COLOR_SLIDER_RECT, B_RGB32, true);

	fBgBitmap->Lock();
	fBgView = new BView(COLOR_SLIDER_RECT, "", B_FOLLOW_NONE, B_WILL_DRAW);
	fBgBitmap->AddChild(fBgView);
	fBgView->SetOrigin(8.0, 2.0);
	fBgBitmap->Unlock();

	fUpdatePort = create_port(100, "color slider update port");

	fUpdateThread = spawn_thread(&ColorSlider::UpdateThread,
		"color slider update thread", 10, this);
	resume_thread(fUpdateThread);

	Update(2);
}


ColorSlider::~ColorSlider() {
	if (fUpdatePort)
		delete_port(fUpdatePort);

	if (fUpdateThread)
		kill_thread(fUpdateThread);
}


void
ColorSlider::AttachedToWindow()
{
	BControl::AttachedToWindow();

	if (Parent())
		SetViewColor(Parent()->ViewColor());
	else
		SetViewColor(216, 216, 216);

	fBgBitmap->Lock();
	fBgView->SetHighColor(ViewColor());
	fBgView->FillRect(COLOR_SLIDER_RECT.OffsetToCopy(0.0, -2.0));
	fBgBitmap->Unlock();

	Update(2);
}


void
ColorSlider::Draw(BRect updateRect)
{
	Update(0);
}


status_t
ColorSlider::Invoke(BMessage* message)
{
	if (!message)
		message = Message();

	message->RemoveName("value");

	switch (fColorMode) {
		case R_SELECTED:
		case G_SELECTED:
		case B_SELECTED:
			message->AddFloat("value", 1.0 - (float)Value() / 255);
			break;

		case H_SELECTED:
			message->AddFloat("value", (1.0 - (float)Value() / 255) * 6);
			break;

		case S_SELECTED:
		case V_SELECTED:
			message->AddFloat("value", 1.0 - (float)Value() / 255);
			break;
	}

	return BControl::Invoke(message);
}


int32
ColorSlider::UpdateThread(void* data)
{
	// initialization

	ColorSlider *colorSlider = (ColorSlider *)data;

	BLooper *looper = colorSlider->Looper();

	if (looper)
		looper->Lock();

	BBitmap *bitmap = colorSlider->fBgBitmap;
	BView *view = colorSlider->fBgView;
	port_id port = colorSlider->fUpdatePort;

	if (looper)
		looper->Unlock();

	// draw

	float h, s, v, r, g, b;
	int R, G, B;

	int32 msg_code;
	char msg_buffer;

	while (true) {
		port_info info;

		do {
			read_port(port, &msg_code, &msg_buffer, sizeof(msg_buffer));
			get_port_info(port, &info);
		} while (info.queue_count);

		if (looper)
			looper->Lock();

		uint colormode = colorSlider->fColorMode;
		float fixedvalue1 = colorSlider->fFixedValue1;
		float fixedvalue2 = colorSlider->fFixedValue2;

		if (looper)
			looper->Unlock();

		bitmap->Lock();

		view->BeginLineArray(256);

		switch (colormode) {
			case R_SELECTED:
			{
				G = round(fixedvalue1 * 255);
				B = round(fixedvalue2 * 255);
				for (int R = 0; R < 256; ++R)
					DrawColorLine(view, R, R, G, B);
				break;
			}

			case G_SELECTED:
			{
				R = round(fixedvalue1 * 255);
				B = round(fixedvalue2 * 255);
				for (int G = 0; G < 256; ++G)
					DrawColorLine(view, G, R, G, B);
				break;
			}
			
			case B_SELECTED:
			{
				R = round(fixedvalue1 * 255);
				G = round(fixedvalue2 * 255);
				for (int B = 0; B < 256; ++B)
					DrawColorLine(view, B, R, G, B);
				break;
			}

			case H_SELECTED:
			{
				s = fixedvalue1;
				v = fixedvalue2;
				for (int y = 0; y < 256; ++y) {
					HSV_to_RGB((float)y * 6.0 / 255.0, s, v, r, g, b);
					DrawColorLine(view, y, r * 255, g * 255, b * 255);
				}
				break;
			}

			case S_SELECTED:
			{
				h = fixedvalue1;
				v = fixedvalue2;
				for (int y = 0; y < 256; ++y)
				{
					HSV_to_RGB(h, (float)y / 255, v, r, g, b);
					DrawColorLine(view, y, r * 255, g * 255, b * 255 );
				}
				break;
			}
			
			case V_SELECTED:
			{
				h = fixedvalue1;
				s = fixedvalue2;
				for (int y = 0; y < 256; ++y) {
					HSV_to_RGB(h, s, (float)y / 255, r, g, b);
					DrawColorLine(view, y, r * 255, g * 255, b * 255);
				}
				break;
			}
		}
	
		view->EndLineArray();
		view->Sync();
		bitmap->Unlock();

		looper = colorSlider->Looper();
		if (looper && looper->Lock()) {
			colorSlider->Update(1);
			looper->Unlock();
		}
	}

	return 0;
}


void
ColorSlider::Update(int depth)
{
	// depth
	// 0 = not onscreen
	// 1 = bitmap 1
	// 2 = bitmap 0

	if (depth == 2) {
		write_port(fUpdatePort, MSG_UPDATE, NULL, 0);
		return;
	}

	if (Parent() == NULL)
		return;

	rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color shadow = tint_color(background, B_DARKEN_1_TINT);
	rgb_color darkShadow = tint_color(background, B_DARKEN_3_TINT);
	rgb_color light = tint_color(background, B_LIGHTEN_MAX_TINT);

	if (depth >= 1) {
		fBgBitmap->Lock();

		BRect bounds(-8.0, -2.0, fBgView->Bounds().right - 8.0,
			fBgView->Bounds().bottom - 2.0);

		bounds.InsetBy(6.0, 0.0);

		// Frame
		fBgView->BeginLineArray(4);
		fBgView->AddLine(BPoint(bounds.left, bounds.bottom),
			BPoint(bounds.left, bounds.top), shadow);
		fBgView->AddLine(BPoint(bounds.left + 1.0, bounds.top),
			BPoint(bounds.right, bounds.top), shadow);
		fBgView->AddLine(BPoint(bounds.right, bounds.top + 1.0),
			BPoint(bounds.right, bounds.bottom), light);
		fBgView->AddLine(BPoint(bounds.right - 1.0, bounds.bottom),
			BPoint(bounds.left + 1.0, bounds.bottom), light);
		fBgView->EndLineArray();
		bounds.InsetBy(1.0, 1.0);
	
		fBgView->BeginLineArray(4);
		fBgView->AddLine(BPoint(bounds.left, bounds.bottom),
			BPoint(bounds.left, bounds.top), darkShadow);
		fBgView->AddLine(BPoint(bounds.left + 1.0, bounds.top),
			BPoint(bounds.right, bounds.top), darkShadow);
		fBgView->AddLine(BPoint(bounds.right, bounds.top + 1.0),
			BPoint(bounds.right, bounds.bottom), background);
		fBgView->AddLine(BPoint(bounds.right - 1.0, bounds.bottom),
			BPoint(bounds.left + 1.0, bounds.bottom), background);
		fBgView->EndLineArray();
		bounds.InsetBy(-7.0, -1.0);

		// Clear the area to the left and right of the slider
		fBgView->SetHighColor(background);
		fBgView->FillRect(BRect(bounds.left, bounds.top, bounds.left + 5.0,
			bounds.bottom));
		fBgView->FillRect(BRect(bounds.right - 5.0, bounds.top, bounds.right,
			bounds.bottom));

		// Marker
		fBgView->SetHighColor(0, 0, 0);
		float value = Value();

		fBgView->StrokeLine(BPoint(bounds.left, value - 2.0),
			BPoint(bounds.left + 5.0, value));
		fBgView->StrokeLine(BPoint(bounds.left, value + 2.0));
		fBgView->StrokeLine(BPoint(bounds.left, value - 2.0));

		fBgView->StrokeLine(BPoint(bounds.right, value - 2.0),
			BPoint(bounds.right - 5.0, value));
		fBgView->StrokeLine(BPoint(bounds.right, value + 2.0));
		fBgView->StrokeLine(BPoint(bounds.right, value - 2.0));

		fBgView->Sync();

		fBgBitmap->Unlock();
	}

	DrawBitmap(fBgBitmap);
}


void
ColorSlider::DrawColorLine(BView *view, float y, int r, int g, int b)
{
	rgb_color color = {r, g, b, 255};
	y = 255.0 - y;

	view->AddLine(BPoint(0.0, y), BPoint(19.0, y), color);
}


void
ColorSlider::MouseDown(BPoint where)
{
	Window()->Activate();

	fMouseDown = true;
	SetMouseEventMask(B_POINTER_EVENTS,
		B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);

	SetValue((int)where.y - 2);
	Invoke();
}


void
ColorSlider::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	if (!fMouseDown || message == NULL) {
		BView::MouseMoved(where, code, message);
		return;
	}

	SetValue((int)where.y - 2);
	Invoke();
}


void
ColorSlider::MouseUp(BPoint where)
{
	fMouseDown = false;
}


void
ColorSlider::SetValue(int32 value)
{
	value = max_c(min_c(value, 255), 0);
	BControl::SetValue(value);

	Update(2);
}


void
ColorSlider::SetModeAndValues(color_mode mode, float value1,
	float value2)
{
	float R(0), G(0), B(0);
	float h(0), s(0), v(0);

	fBgBitmap->Lock();

	switch (fColorMode) {
		case R_SELECTED:
			R = 255 - Value();
			G = round(fFixedValue1 * 255.0);
			B = round(fFixedValue2 * 255.0);
			break;

		case G_SELECTED:
			R = round(fFixedValue1 * 255.0);
			G = 255 - Value();
			B = round(fFixedValue2 * 255.0);
			break;

		case B_SELECTED:
			R = round(fFixedValue1 * 255.0);
			G = round(fFixedValue2 * 255.0);
			B = 255 - Value();
			break;

		case H_SELECTED:
			h = (1.0 - (float)Value() / 255.0) * 6.0;
			s = fFixedValue1;
			v = fFixedValue2;
			break;

		case S_SELECTED:
			h = fFixedValue1;
			s = 1.0 - (float)Value() / 255.0;
			v = fFixedValue2;
			break;

		case V_SELECTED:
			h = fFixedValue1;
			s = fFixedValue2;
			v = 1.0 - (float)Value() / 255.0;
			break;
	}

	if ((fColorMode & (H_SELECTED | S_SELECTED | V_SELECTED)) != 0) {
		HSV_to_RGB(h, s, v, R, G, B);
		R *= 255.0;
		G *= 255.0;
		B *= 255.0;
	}

	rgb_color color = { round(R), round(G), round(B), 255 };

	fColorMode = mode;
	SetOtherValues(value1, value2);
	fBgBitmap->Unlock();

	SetMarkerToColor(color);
	Update(2);
}


void
ColorSlider::SetOtherValues(float value1, float value2)
{
	fFixedValue1 = value1;
	fFixedValue2 = value2;

	Update(2);
}


void
ColorSlider::SetMarkerToColor(rgb_color color)
{
	float h, s, v;
	if ((fColorMode & (H_SELECTED | S_SELECTED | V_SELECTED)) != 0) {
		RGB_to_HSV((float)color.red / 255.0, (float)color.green / 255.0,
			(float)color.blue / 255.0, h, s, v);
	}

	switch (fColorMode) {
		case R_SELECTED:
			SetValue(255 - color.red);
			break;

		case G_SELECTED:
			SetValue(255 - color.green);
			break;

		case B_SELECTED:
			SetValue(255 - color.blue);
			break;

		case H_SELECTED:
			SetValue(255.0 - round(h / 6.0 * 255.0));
			break;

		case S_SELECTED:
			SetValue(255.0 - round(s * 255.0));
			break;
		
		case V_SELECTED:
			SetValue(255.0 - round(v * 255.0));
			break;
	}

	Update(1);
}
