/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "WebSafeSelector.h"

#include <Handler.h>
#include <Message.h>
#include <Window.h>


WebSafeSelector::WebSafeSelector(BRect frame)
	:
	BView(frame, "WebSafeSelector", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW)
{
}


WebSafeSelector::~WebSafeSelector()
{
}


void
WebSafeSelector::Draw(BRect updateRect)
{
	rgb_color border = (rgb_color){ 184, 184, 184 };
	SetHighColor(border);
	StrokeRect(Bounds());

	SetHighColor(fColor);
	FillRect(Bounds().InsetByCopy(1.0, 1.0));
}


void
WebSafeSelector::MouseDown(BPoint where)
{
	BMessage message;
	message.AddData("RGBColor", B_RGB_COLOR_TYPE, &fColor, sizeof(fColor));

	BHandler* colorpreview;
	if ((colorpreview = (BHandler*)Window()->FindView("color preview")))
		Window()->PostMessage(&message, colorpreview);
}


rgb_color
WebSafeSelector::GetColor() const
{
	return fColor;
}


void
WebSafeSelector::SetColor(long int c)
{
	rgb_color color;
	color.red   = (c >> 16) & 255;
	color.green = (c >> 8) & 255;
	color.blue  = c & 255;
	SetColor(color);
}


void
WebSafeSelector::SetColor(rgb_color color)
{
	color.alpha = 255;
	fColor = color;
	if (Window())
		Draw(Bounds());
}
