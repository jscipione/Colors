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


OutOfGamutSelector::OutOfGamutSelector()
	:
	ColorSelector()
{
	// Under construction
	//Show();
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
		rgb_color warning;
		rgb_color accent;

		if (fMouseOver) {
			warning = (rgb_color){ 239, 197, 27, 255 };
			accent = tint_color(warning, B_DARKEN_1_TINT);
		} else {
			warning = ViewColor();
			accent = tint_color(black, B_LIGHTEN_2_TINT);
		}

		BRect bounds = INDICATOR_RECT;

		BPoint point1(bounds.LeftTop() + BPoint(0, 11));
		BPoint point2(bounds.LeftTop() + BPoint(10, 11));
		BPoint point3(bounds.LeftTop() + BPoint(5, 1));

		SetHighColor(warning);
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

		SetHighColor(accent);
		StrokeLine(point7, point8);
	}
}


void
OutOfGamutSelector::Hide() {
	ColorSelector::Hide();
	SetToolTip("");
}


void
OutOfGamutSelector::Show() {
	ColorSelector::Show();
	SetToolTip("Click to select an in gamut color for printing");
}


void
OutOfGamutSelector::SetColor(rgb_color color)
{
	// stub, set to closest in-gamut color here
	if (true) {
		if (!IsHidden())
			Hide();
	} else if (IsHidden())
		Show();

	fColor = color;
	if (Window() && !IsHidden())
		Draw(COLOR_RECT);
}
