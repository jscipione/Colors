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


#include "ColorPreview.h"

#include <iostream>
#include <stdio.h>
#include <stdio.h>

#include <Alignment.h>
#include <Bitmap.h>
#include <MessageRunner.h>
#include <Size.h>
#include <String.h>
#include <Window.h>


ColorPreview::ColorPreview()
	:
	BControl("color preview", "", new BMessage(MSG_COLOR_PREVIEW), B_WILL_DRAW),
	fMouseDown(false),
	fMessageRunner(0)
{
	SetExplicitMinSize(BSize(56.0, 50.0));
	SetExplicitMaxSize(BSize(56.0, 50.0));
	SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_VERTICAL_CENTER));
}


ColorPreview::~ColorPreview()
{
}


void
ColorPreview::Draw(BRect updateRect)
{
	rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
	rgb_color shadow = tint_color(background, B_DARKEN_1_TINT);
	rgb_color darkShadow = tint_color(background, B_DARKEN_3_TINT);
	rgb_color light = tint_color(background, B_LIGHTEN_MAX_TINT);

	BRect bounds(Bounds());

	BeginLineArray(4);
	AddLine(BPoint(bounds.left, bounds.bottom),
		BPoint(bounds.left, bounds.top), shadow);
	AddLine(BPoint(bounds.left + 1.0, bounds.top),
		BPoint(bounds.right, bounds.top), shadow);
	AddLine(BPoint(bounds.right, bounds.top + 1.0),
		BPoint(bounds.right, bounds.bottom), light);
	AddLine(BPoint(bounds.right - 1.0, bounds.bottom),
		BPoint(bounds.left + 1.0, bounds.bottom), light);
	EndLineArray();
	bounds.InsetBy(1.0, 1.0);

	BeginLineArray(4);
	AddLine(BPoint(bounds.left, bounds.bottom),
		BPoint(bounds.left, bounds.top), darkShadow);
	AddLine(BPoint(bounds.left + 1.0, bounds.top),
		BPoint(bounds.right, bounds.top), darkShadow);
	AddLine(BPoint(bounds.right, bounds.top + 1.0),
		BPoint(bounds.right, bounds.bottom), background);
	AddLine(BPoint(bounds.right - 1.0, bounds.bottom),
		BPoint(bounds.left + 1.0, bounds.bottom), background);
	EndLineArray();
	bounds.InsetBy(1.0, 1.0);

	bounds.bottom = bounds.top + bounds.Height() / 2.0;
	SetHighColor(fColor);
	FillRect(bounds);

	bounds.top = bounds.bottom + 1;
	bounds.bottom = Bounds().bottom - 2.0;
	SetHighColor(fOldColor);
	FillRect(bounds);
}


status_t
ColorPreview::Invoke(BMessage *message)
{
	if (message == NULL)
		message = Message();

	message->RemoveName("selected_color");
	message->AddData("selected_color", B_RGB_COLOR_TYPE, &fColor,
		sizeof(fColor));

	return BControl::Invoke(message);
}


void
ColorPreview::MessageReceived(BMessage *message)
{
	if (message->what == MSG_MESSAGERUNNER) {
		BPoint where;
		uint32 buttons;
		GetMouse(&where, &buttons);

		_DragColor(where);
	} else {
		char *nameFound;
		type_code typeFound;
		if (message->GetInfo(B_RGB_COLOR_TYPE, 0, &nameFound, &typeFound)
				!= B_OK) {
			BControl::MessageReceived(message);
			return;
		}

		rgb_color* color;
		ssize_t numBytes;
		message->FindData(nameFound, typeFound, (const void **)&color,
			&numBytes);

		BPoint where;
		bool droppedOnNewArea = false;
		if (message->FindPoint("_drop_point_", &where) == B_OK) {
			ConvertFromScreen(&where);
			if (where.y > Bounds().top + (Bounds().IntegerHeight() >> 1))
				droppedOnNewArea = true;
		}

		if (droppedOnNewArea)
			SetNewColor(*color);
		else
			SetColor(*color);

		Invoke();
	}
}


void
ColorPreview::MouseDown(BPoint where)
{
	Window()->Activate();

	fMouseDown = true;
	fMessageRunner = new BMessageRunner(
		this, new BMessage(MSG_MESSAGERUNNER), 300000, 1);

	SetMouseEventMask(B_POINTER_EVENTS,
		B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);

	BRect rect = Bounds().InsetByCopy(2.0, 2.0);
	rect.top = rect.bottom / 2 + 1;

	if (rect.Contains(where)) {
		fColor = fOldColor;
		Draw(Bounds());
		Invoke();
	}
}


void
ColorPreview::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	if (fMouseDown)
		_DragColor(where);
}


void
ColorPreview::MouseUp(BPoint where)
{
	delete fMessageRunner;
	fMessageRunner = 0;

	fMouseDown = false;
	BControl::MouseUp(where);
}


void
ColorPreview::SetColor(rgb_color color)
{
	color.alpha = 255;
	fColor = color;

	Invalidate();
}


void
ColorPreview::SetNewColor(rgb_color color)
{
	fColor = color;
	fOldColor = color;

	Invalidate();
}


// #pragma mark -


void
ColorPreview::_DragColor(BPoint where)
{
	char hexstr[7];
	sprintf(hexstr, "#%.2X%.2X%.2X", fColor.red, fColor.green, fColor.blue);

	BMessage message(B_PASTE);
	message.AddData("text/plain", B_MIME_TYPE, &hexstr, sizeof(hexstr));
	message.AddData("RGBColor", B_RGB_COLOR_TYPE, &fColor, sizeof(fColor));

	BRect rect(0.0, 0.0, 20.0, 20.0);

	BBitmap* bitmap = new BBitmap(rect, B_RGB32, true);
	bitmap->Lock();

	BView* view = new BView(rect, "", B_FOLLOW_NONE, B_WILL_DRAW);
	bitmap->AddChild(view);

	view->SetHighColor(B_TRANSPARENT_COLOR);
	view->FillRect(view->Bounds());

	++rect.top;
	++rect.left;

	view->SetHighColor(0, 0, 0, 100);
	view->FillRect(rect);
	rect.OffsetBy(-1.0, -1.0);

	view->SetHighColor(min_c(255, 1.2 * fColor.red + 40),
		min_c(255, 1.2 * fColor.green + 40),
		min_c(255, 1.2 * fColor.blue + 40));
	view->StrokeRect(rect);

	++rect.left;
	++rect.top;

	view->SetHighColor(0.8 * fColor.red, 0.8 * fColor.green, 0.8 * fColor.blue);
	view->StrokeRect(rect);

	--rect.right;
	--rect.bottom;

	view->SetHighColor(fColor.red, fColor.green, fColor.blue);
	view->FillRect(rect);
	view->Flush();

	bitmap->Unlock();

	DragMessage(&message, bitmap, B_OP_ALPHA, BPoint(14.0, 14.0));

	MouseUp(where);
}
