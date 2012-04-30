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


#include "EyeDropper.h"

#include <Alignment.h>
#include <Application.h>
#include <Bitmap.h>
#include <Resources.h>
#include <Size.h>
#include <Window.h>


EyeDropper::EyeDropper()
	:
	BControl("eye dropper", "", new BMessage(MSG_EYEDROPPER), B_WILL_DRAW),
	fMouseDown(false),
	fButtonDown(false)
{
	SetExplicitMinSize(BSize(56.0, 19.0));
	SetExplicitMaxSize(BSize(56.0, 19.0));
	SetExplicitAlignment(BAlignment(B_ALIGN_RIGHT, B_ALIGN_VERTICAL_CENTER));
	SetDrawingMode(B_OP_ALPHA);

	fImage[0] = FetchImage("pipette0");
	fImage[1] = FetchImage("pipette1");
}


EyeDropper::~EyeDropper()
{
	delete fImage[0];
	delete fImage[1];
}


void
EyeDropper::Draw(BRect updateRect)
{
	DrawBitmap(fImage[fButtonDown]);
}


void
EyeDropper::MouseDown(BPoint where)
{
	Window()->Activate();

	fButtonDown = true;
	fMouseDown = true;
	Draw(Bounds());

	SetMouseEventMask(B_POINTER_EVENTS,
		B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS );
}


void
EyeDropper::MouseMoved(BPoint where, uint32 code, const BMessage* message)
{
	if (!fMouseDown || message == NULL)
		return;

	if (Bounds().Contains(where)) {
		if (fButtonDown)
			return;

		fButtonDown = true;
	} else {
		if (!fButtonDown)
			return;

		fButtonDown = false;
	}

	Draw(Bounds());
}


void
EyeDropper::MouseUp(BPoint where)
{
	fMouseDown = false;

	if (fButtonDown) {
		Invoke();
		fButtonDown = false;
	}

	Draw(Bounds());
}


// #pragma mark -


BBitmap*
EyeDropper::FetchImage(const char* imgname)
{
	BResources *resource = be_app->AppResources();

	size_t length;
	BMessage message;
	char* buffer = (char *)resource->LoadResource('BBMP', imgname, &length);

	if (buffer) {
		message.Unflatten(buffer);
		return new BBitmap(&message);
	}

	return NULL;
}
