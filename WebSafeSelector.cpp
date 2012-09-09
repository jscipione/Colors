/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "WebSafeSelector.h"

#include <InterfaceDefs.h>
#include <Point.h>
#include <Polygon.h>
#include <Window.h>

#define INDICATOR_RECT BRect(4.0, 0.0, 16.0, 12.0)
#define COLOR_RECT BRect(0.0, 18.0, 18.0, 36.0)


rgb_color
round_to_nearest_websafe_color(rgb_color color)
{
	color.red = (uint8)(floor((color.red + (0x33 / 2)) / 0x33) * 0x33);
	color.green = (uint8)(floor((color.green + (0x33 / 2)) / 0x33) * 0x33);
	color.blue = (uint8)(floor((color.blue + (0x33 / 2)) / 0x33) * 0x33);

	return color;
}


WebSafeSelector::WebSafeSelector()
	:
	BControl("WebSafeSelector", "", new BMessage(MSG_WEBSAFE), B_WILL_DRAW)
{
}


WebSafeSelector::~WebSafeSelector()
{
}


void
WebSafeSelector::Draw(BRect updateRect)
{
	if (updateRect.Intersects(COLOR_RECT)) {
		rgb_color background = ui_color(B_PANEL_BACKGROUND_COLOR);
		rgb_color shadow = tint_color(background, B_DARKEN_1_TINT);
		rgb_color darkShadow = tint_color(background, B_DARKEN_3_TINT);
		rgb_color light = tint_color(background, B_LIGHTEN_MAX_TINT);

		BRect bounds = COLOR_RECT;

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

		SetHighColor(fColor);
		FillRect(bounds);
	}

	if (updateRect.Intersects(INDICATOR_RECT)) {
		rgb_color black = (rgb_color){ 0, 0, 0 };
		rgb_color light = tint_color(black, B_LIGHTEN_MAX_TINT);
		rgb_color medium = tint_color(black, B_LIGHTEN_2_TINT);
		rgb_color dark = tint_color(black, B_LIGHTEN_1_TINT);

		BRect bounds = INDICATOR_RECT;

		BPoint pointList[4];
		pointList[0] = bounds.LeftTop() + BPoint(0, 3);
		pointList[1] = bounds.LeftTop() + BPoint(0, 9);
		pointList[2] = bounds.LeftTop() + BPoint(5, 12);
		pointList[3] = bounds.LeftTop() + BPoint(5, 6);

		BPolygon* rhombus1 = new BPolygon(pointList, 4);
		SetHighColor(dark);
		FillPolygon(rhombus1);
		SetHighColor(black);
		StrokePolygon(rhombus1);
		delete rhombus1;

		pointList[0] = bounds.LeftTop() + BPoint(5, 6);
		pointList[1] = bounds.LeftTop() + BPoint(5, 12);
		pointList[2] = bounds.LeftTop() + BPoint(10, 9);
		pointList[3] = bounds.LeftTop() + BPoint(10, 3);

		BPolygon* rhombus2 = new BPolygon(pointList, 4);
		SetHighColor(light);
		FillPolygon(rhombus2);
		SetHighColor(black);
		StrokePolygon(rhombus2);
		delete rhombus2;

		pointList[0] = bounds.LeftTop() + BPoint(0, 3);
		pointList[1] = bounds.LeftTop() + BPoint(5, 6);
		pointList[2] = bounds.LeftTop() + BPoint(10, 3);
		pointList[3] = bounds.LeftTop() + BPoint(5, 0);

		BPolygon* rhombus3 = new BPolygon(pointList, 4);
		SetHighColor(medium);
		FillPolygon(rhombus3);
		SetHighColor(black);
		StrokePolygon(rhombus3);
		delete rhombus3;
	}
}


status_t
WebSafeSelector::Invoke(BMessage *message)
{
	if (message == NULL)
		message = Message();

	message->RemoveName("selected_color");
	message->AddData("selected_color", B_RGB_COLOR_TYPE, &fColor,
		sizeof(fColor));

	return BControl::Invoke(message);
}


void
WebSafeSelector::MouseDown(BPoint where)
{
	Window()->Activate();

	BMessage message;
	message.AddData("RGBColor", B_RGB_COLOR_TYPE, &fColor, sizeof(fColor));

	Invoke();
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
	
	if ((color.red % 0x33) == 0 &&
		(color.green % 0x33) == 0 &&
		(color.blue % 0x33) == 0) {
		if (!IsHidden())
			Hide();
	} else if (IsHidden())
		Show();

	color = round_to_nearest_websafe_color(color);
	color.alpha = 255;
	fColor = color;
	if (Window() && !IsHidden())
		Draw(COLOR_RECT);
}
