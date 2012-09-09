/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 *
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "OutOfGamutSelector.h"

#include <Control.h>
#include <InterfaceDefs.h>
#include <Point.h>
#include <Polygon.h>
#include <Window.h>

#define COLOR_RECT BRect(0.0, 18.0, 18.0, 36.0)
#define INDICATOR_RECT BRect(4.0, 0.0, 16.0, 12.0)


typedef struct cmyk_color {
	float cyan;
	float magenta;
	float yellow;
	float black;
} cmyk_color;

cmyk_color rgb2cmyk(uint8 red, uint8 green, uint8 blue);
rgb_color cmyk2rgb(int cyan, int magenta, int yellow, int black);


cmyk_color
rgb2cmyk(uint8 red, uint8 green, uint8 blue)
{
	float c = 1 - (red / 255.0f);
	float m = 1 - (green / 255.0f);
	float y = 1 - (blue / 255.0f);
	float k = MIN(MIN(c, m), y);

	cmyk_color color;
	color.cyan    = 100.0f * (c - k) / (1 - k);
	color.magenta = 100.0f * (m - k) / (1 - k);
	color.yellow  = 100.0f * (y - k) / (1 - k);
	color.black   = 100.0f * k;

	return color;
}


rgb_color
cmyk2rgb(float cyan, float magenta, float yellow, float black)
{
	cyan = MIN(255.0f, cyan + black);
	magenta = MIN(255.0f, magenta + black);
	yellow = MIN(255.0f, yellow + black);

	rgb_color color;
	color.red   = (uint8)(255 - cyan);
	color.green = (uint8)(255 - magenta);
	color.blue  = (uint8)(255 - yellow);

	return color;
}


OutOfGamutSelector::OutOfGamutSelector()
	:
	ColorSelector()
{
}


OutOfGamutSelector::~OutOfGamutSelector()
{
}


void
OutOfGamutSelector::Draw(BRect updateRect)
{
	ColorSelector::Draw(updateRect);

	if (fIsHidden) {
		SetHighColor(ViewColor());
		FillRect(updateRect);
		return;
	}

	if (updateRect.Intersects(INDICATOR_RECT)) {
		rgb_color black = (rgb_color){ 0, 0, 0 };
		rgb_color medium = tint_color(black, B_LIGHTEN_2_TINT);

		BRect bounds = INDICATOR_RECT;

		BPoint point1(bounds.LeftTop() + BPoint(0, 11));
		BPoint point2(bounds.LeftTop() + BPoint(10, 11));
		BPoint point3(bounds.LeftTop() + BPoint(5, 1));

		SetHighColor(ViewColor());
		FillTriangle(point1, point2, point3);

		SetHighColor(black);
		StrokeTriangle(point1, point2, point3);

		BPoint point4 = point3 + BPoint(0, 4);
		BPoint point5 = point4 + BPoint(0, 2);
		BPoint point6 = point5 + BPoint(0, 2);

		StrokeLine(point4, point5);
		StrokeLine(point6, point6);

		BPoint point7 = point1 + BPoint(1, -1);
		BPoint point8 = point2 + BPoint(-1, -1);

		SetHighColor(medium);
		StrokeLine(point7, point8);
	}
}


void
OutOfGamutSelector::SetColor(rgb_color color)
{
	cmyk_color cmyk = rgb2cmyk(color.red, color.green, color.blue);
	rgb_color rgb = cmyk2rgb(cmyk.cyan, cmyk.magenta, cmyk.yellow, cmyk.black);
	if (rgb.red == color.red &&
		rgb.green == color.green &&
		rgb.blue == color.blue) {
		if (!IsHidden())
			Hide();
	} else if (IsHidden())
		Show();

	fColor = rgb;
	if (Window() && !IsHidden())
		Draw(COLOR_RECT);
}
