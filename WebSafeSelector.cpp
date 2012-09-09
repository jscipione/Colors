/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "WebSafeSelector.h"

#include <Control.h>
#include <InterfaceDefs.h>
#include <Point.h>
#include <Polygon.h>
#include <Window.h>

#define COLOR_RECT BRect(0.0, 18.0, 18.0, 36.0)
#define INDICATOR_RECT BRect(4.0, 0.0, 16.0, 12.0)


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
	ColorSelector()
{
}


WebSafeSelector::~WebSafeSelector()
{
}


void
WebSafeSelector::Draw(BRect updateRect)
{
	ColorSelector::Draw(updateRect);

	if (fIsHidden) {
		SetHighColor(ViewColor());
		FillRect(updateRect);
		return;
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
