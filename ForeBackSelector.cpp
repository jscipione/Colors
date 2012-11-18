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


#include "ForeBackSelector.h"

#include <InterfaceDefs.h>
#include <Rect.h>
#include <Window.h>

#include "ColorWell.h"

#define COLOR_RECT_FORE BRect(0.0, 0.0, 27.0, 27.0)
#define COLOR_RECT_BACK BRect(0.0, 0.0, 27.0, 27.0).OffsetToCopy(13.0, 13.0)

#define DEFAULT_RECT_FORE BRect(0.0, 0.0, 7.0, 7.0).OffsetToCopy(0.0, 30.0)
#define DEFAULT_RECT_BACK BRect(0.0, 0.0, 7.0, 7.0).OffsetToCopy(3.0, 33.0)

#define SWITCHER_RECT BRect(0.0, 0.0, 9.0, 9.0).OffsetToCopy(30.0, 0.0)


ForeBackSelector::ForeBackSelector(BRect frame)
	:
	BView(frame, "fore back selector", B_FOLLOW_TOP | B_FOLLOW_LEFT,
		B_WILL_DRAW | B_DRAW_ON_CHILDREN)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	fForeColorWell = new ColorWell(COLOR_RECT_FORE);
	AddChild(fForeColorWell);
	fBackColorWell = new ColorWell(COLOR_RECT_FORE);
	AddChild(fBackColorWell);
}


ForeBackSelector::~ForeBackSelector()
{
}


void
ForeBackSelector::Draw(BRect updateRect)
{
	rgb_color border = (rgb_color){ 184, 184, 184 };
	rgb_color switcher = (rgb_color){ 100, 100, 100 };
	rgb_color white = (rgb_color){ 255, 255, 255 };
	rgb_color black = (rgb_color){ 0, 0, 0 };

	// Draw the switcher
	SetHighColor(switcher);
	StrokeArc(SWITCHER_RECT.OffsetByCopy(-2.0, 2.0), 0.0, 90.0);
	StrokeTriangle(BPoint(SWITCHER_RECT.left + 1.0, SWITCHER_RECT.top + 3.0),
		BPoint(SWITCHER_RECT.left + 3.0, SWITCHER_RECT.top + 1.0),
		BPoint(SWITCHER_RECT.left + 3.0, SWITCHER_RECT.top + 5.0));
	StrokeTriangle(BPoint(SWITCHER_RECT.right - 2.0, SWITCHER_RECT.bottom),
		BPoint(SWITCHER_RECT.right, SWITCHER_RECT.bottom - 2.0),
		BPoint(SWITCHER_RECT.right - 4.0, SWITCHER_RECT.bottom - 2.0));

	// draw the background default rect (white)
	SetHighColor(border);
	StrokeRect(DEFAULT_RECT_BACK);
	SetHighColor(white);
	FillRect(DEFAULT_RECT_BACK.InsetByCopy(1.0, 1.0));

	// draw the foreground default rect (black)
	SetDrawingMode(B_OP_OVER);
	SetHighColor(border);
	StrokeRect(DEFAULT_RECT_FORE);
	SetHighColor(black);
	FillRect(DEFAULT_RECT_FORE.InsetByCopy(1.0, 1.0));
	SetDrawingMode(B_OP_COPY);
}


void
ForeBackSelector::DrawAfterChildren(BRect updateRect)
{
	rgb_color border = (rgb_color){ 184, 184, 184 };
	rgb_color white = (rgb_color){ 255, 255, 255 };

	// draw the background color rect
	SetHighColor(border);
	StrokeRect(COLOR_RECT_BACK);
	SetHighColor(white);
	StrokeRect(COLOR_RECT_BACK.InsetByCopy(1.0, 1.0));
	SetHighColor(fBackColorWell->GetColor());
	FillRect(COLOR_RECT_BACK.InsetByCopy(2.0, 2.0));

	// draw the foreground color rect
	SetDrawingMode(B_OP_OVER);
	SetHighColor(border);
	StrokeRect(COLOR_RECT_FORE);
	SetHighColor(white);
	StrokeRect(COLOR_RECT_FORE.InsetByCopy(1.0, 1.0));
	SetHighColor(fForeColorWell->GetColor());
	FillRect(COLOR_RECT_FORE.InsetByCopy(2.0, 2.0));
	SetDrawingMode(B_OP_COPY);
}


void
ForeBackSelector::MessageReceived(BMessage *message)
{
	char* nameFound;
	type_code typeFound;
	if (message->GetInfo(B_RGB_COLOR_TYPE, 0, &nameFound, &typeFound)
			!= B_OK) {
		BView::MessageReceived(message);
		return;
	}

	BPoint where;
	uint32 buttons;
	GetMouse(&where, &buttons);

	if (COLOR_RECT_FORE.Contains(where))
		fForeColorWell->MessageReceived(message);
	else if (COLOR_RECT_BACK.Contains(where))
		fBackColorWell->MessageReceived(message);

	DrawAfterChildren(Bounds());
}


void
ForeBackSelector::MouseDown(BPoint where)
{
	Window()->Activate();

	if ((DEFAULT_RECT_FORE | DEFAULT_RECT_BACK).Contains(where)) {
		fForeColorWell->SetColor(0x000000);
		fBackColorWell->SetColor(0xffffff);
		DrawAfterChildren(Bounds());
		BView::MouseDown(where);
	} else if (SWITCHER_RECT.Contains(where)) {
		rgb_color foreColor = fForeColorWell->GetColor();
		rgb_color backColor = fBackColorWell->GetColor();
		fForeColorWell->SetColor(backColor);
		fBackColorWell->SetColor(foreColor);
		DrawAfterChildren(Bounds());
		BView::MouseDown(where);
	} else if (COLOR_RECT_FORE.Contains(where))
		fForeColorWell->MouseDown(where);
	else if (COLOR_RECT_BACK.Contains(where))
		fBackColorWell->MouseDown(where);
	else
		BView::MouseDown(where);
}


void
ForeBackSelector::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	if (COLOR_RECT_FORE.Contains(where))
		fForeColorWell->MouseMoved(where, code, message);
	else if (COLOR_RECT_BACK.Contains(where))
		fBackColorWell->MouseMoved(where, code, message);
	else
		BView::MouseMoved(where, code, message);
}


void
ForeBackSelector::MouseUp(BPoint where)
{
	if (COLOR_RECT_FORE.Contains(where))
		fForeColorWell->MouseUp(where);
	else if (COLOR_RECT_BACK.Contains(where))
		fBackColorWell->MouseUp(where);
	else
		BView::MouseUp(where);
}
