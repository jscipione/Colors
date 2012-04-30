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


#include "ColorWell.h"

#include <Application.h>
#include <Bitmap.h>
#include <iostream>
#include <MessageRunner.h>
#include <Resources.h>
#include <stdio.h>
#include <String.h>
#include <stdio.h>
#include <Window.h>


ColorWell::ColorWell(BRect frame)
	:
	BView(frame, "ColorWell", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW),
	fMouseDown(false),
	fMouseOffset(BPoint(0, 0)),
	fMessageRunner(0),
	fGotFirstClick(false)
{
}


ColorWell::~ColorWell()
{
}


void
ColorWell::Draw(BRect updateRect)
{
	rgb_color border = (rgb_color){ 184, 184, 184 };
	SetHighColor(border);
	StrokeRect(Bounds());

	SetHighColor(fColor);
	FillRect(Bounds().InsetByCopy(1.0, 1.0));
}


void
ColorWell::MessageReceived(BMessage* message)
{
	if (message->what == MSG_MESSAGERUNNER) {
		BPoint where;
		uint32 buttons;
		GetMouse(&where, &buttons);

		DragColor(where);
	} else {
		char* nameFound;
		type_code typeFound;
		if (message->GetInfo(B_RGB_COLOR_TYPE, 0, &nameFound, &typeFound)
				!= B_OK) {
			BView::MessageReceived(message);
			return;
		}

		rgb_color* color;
		ssize_t numBytes;
		message->FindData(nameFound, typeFound, (const void **)&color,
			&numBytes);
		SetColor(*color);
	}
}


void
ColorWell::MouseDown(BPoint where)
{
	Window()->Activate();

	long clicks = Window()->CurrentMessage()->FindInt32("clicks");

	if (clicks == 2 && fGotFirstClick) {
		BMessage message;
		message.AddData("RGBColor", B_RGB_COLOR_TYPE, &fColor, sizeof(fColor));

		BHandler* colorpreview;
		if ((colorpreview = (BHandler*)Window()->FindView("color preview")))
			Window()->PostMessage(&message, colorpreview);
	} else {
		fGotFirstClick = (clicks == 1);
		fMouseDown = true;

		bigtime_t doubleClickSpeed;
		if (get_click_speed(&doubleClickSpeed) != B_OK)
			doubleClickSpeed = 300000;

		fMessageRunner = new BMessageRunner(this,
			new BMessage(MSG_MESSAGERUNNER), doubleClickSpeed, 1);

		SetMouseEventMask(B_POINTER_EVENTS,
			B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);
	}
}


void
ColorWell::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	if (fMouseDown)
		DragColor(where);
}


void
ColorWell::MouseUp(BPoint where)
{
	delete fMessageRunner;
	fMessageRunner = 0;

	fMouseDown = false;
	BView::MouseUp(where);
}


rgb_color
ColorWell::GetColor() const
{
	return fColor;
}


void
ColorWell::SetColor(long int c)
{
	rgb_color color;
	color.red   = (c >> 16) & 255;
	color.green = (c >> 8) & 255;
	color.blue  = c & 255;
	SetColor(color);
}


void
ColorWell::SetColor(rgb_color color)
{
	color.alpha = 255;
	fColor = color;
	if (Window())
		Draw(Bounds());
}


// #pragma mark -


void
ColorWell::DragColor(BPoint where)
{
	fGotFirstClick = false;

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

	view->SetHighColor(0.8 * fColor.red, 0.8 * fColor.green,
		0.8 * fColor.blue);
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
