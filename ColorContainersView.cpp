/*
 * Copyright 2009-2013 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *		Werner Freytag <freytag@gmx.de>
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "ColorContainersView.h"

#include <iostream>

#include <Application.h>
#include <Bitmap.h>
#include <InterfaceDefs.h>
#include <LayoutBuilder.h>
#include <Resources.h>
#include <Size.h>
#include <Window.h>

#include "ColorsApplication.h"
#include "ColorContainer.h"
#include "ColorsWindow.h"

#define COLOR_TO_INT(c) (c.red << 16) + (c.green << 8) + (c.blue)


ColorContainersView::ColorContainersView()
	:
	BView("containers view", B_WILL_DRAW),
	fMouseDown(false)
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));

	for (int32 i = 0; i < kColorContainerCount; ++i)
		fColorContainer[i] = new ColorContainer(BRect(0.0, 0.0, 19.0, 19.0));

	BLayoutBuilder::Group<>(this)
		.AddGroup(B_HORIZONTAL, 24.0)
			.AddGrid(1, 1)
				.Add(fColorContainer[0], 0, 0)
				.Add(fColorContainer[1], 1, 0)
				.Add(fColorContainer[2], 2, 0)
				.Add(fColorContainer[3], 3, 0)
				.Add(fColorContainer[4], 4, 0)
				.Add(fColorContainer[5], 5, 0)
				.Add(fColorContainer[6], 6, 0)
				.Add(fColorContainer[7], 7, 0)
				.Add(fColorContainer[8], 8, 0)
				.Add(fColorContainer[9], 9, 0)
				.Add(fColorContainer[10], 10, 0)
				.Add(fColorContainer[11], 11, 0)
				.Add(fColorContainer[12], 12, 0)
				.Add(fColorContainer[13], 13, 0)
				.Add(fColorContainer[14], 14, 0)
				.Add(fColorContainer[15], 15, 0)
				.Add(fColorContainer[16], 16, 0)
				.Add(fColorContainer[17], 17, 0)
				.Add(fColorContainer[18], 18, 0)
				.Add(fColorContainer[19], 19, 0)
				.Add(fColorContainer[20], 0, 1)
				.Add(fColorContainer[21], 1, 1)
				.Add(fColorContainer[22], 2, 1)
				.Add(fColorContainer[23], 3, 1)
				.Add(fColorContainer[24], 4, 1)
				.Add(fColorContainer[25], 5, 1)
				.Add(fColorContainer[26], 6, 1)
				.Add(fColorContainer[27], 7, 1)
				.Add(fColorContainer[28], 8, 1)
				.Add(fColorContainer[29], 9, 1)
				.Add(fColorContainer[30], 10, 1)
				.Add(fColorContainer[31], 11, 1)
				.Add(fColorContainer[32], 12, 1)
				.Add(fColorContainer[33], 13, 1)
				.Add(fColorContainer[34], 14, 1)
				.Add(fColorContainer[35], 15, 1)
				.Add(fColorContainer[36], 16, 1)
				.Add(fColorContainer[37], 17, 1)
				.Add(fColorContainer[38], 18, 1)
				.Add(fColorContainer[39], 19, 1)
				.End()
			.SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING,
				B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING)
			.End();
}


ColorContainersView::~ColorContainersView()
{
}


void
ColorContainersView::AttachedToWindow()
{
	BView::AttachedToWindow();

	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	for (int32 i = 0; i < kColorContainerCount; ++i) {
		long int int_color;
		if (settings->FindInt32("color_well", i, &int_color) == B_OK)
			fColorContainer[i]->SetColor(int_color);
		else
			fColorContainer[i]->SetColor(0xffffff);
	}
}


void
ColorContainersView::Draw(BRect updateRect)
{
	BView::Draw(updateRect);

	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(0, 0, 0, 100);
	StrokeLine(Bounds().RightTop(), Bounds().LeftTop());
	StrokeLine(Bounds().LeftBottom());
	SetHighColor(255, 255, 255, 200);
	StrokeLine(Bounds().RightBottom());
	StrokeLine(Bounds().RightTop());
	SetDrawingMode(B_OP_COPY);
}


void
ColorContainersView::MouseDown(BPoint where)
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
ColorContainersView::MouseMoved(BPoint where, uint32 code, const BMessage *message)
{
	if (fMouseDown) {
		BPoint win_pos = Window()->Frame().LeftTop();
		Window()->MoveTo(win_pos.x + where.x - fMouseOffset.x,
			win_pos.y + where.y - fMouseOffset.y);
	} else
		BView::MouseMoved(where, code, message);
}


void
ColorContainersView::MouseUp(BPoint where)
{
	fMouseDown = false;

	BView::MouseUp(where);
}


void
ColorContainersView::SaveSettings()
{
	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	settings->RemoveName("color_well");
	for (int32 i = 0; i < kColorContainerCount; ++i) {
		settings->AddInt32("color_well",
			COLOR_TO_INT(fColorContainer[i]->GetColor()));
	}
}
