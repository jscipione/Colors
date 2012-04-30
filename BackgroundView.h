/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _BACKGROUND_VIEW_H
#define _BACKGROUND_VIEW_H


#include <View.h>


class BBitmap;

class BackgroundView : public BView {
	public:
									BackgroundView( BRect frame );
		virtual						~BackgroundView();

		virtual	void				AttachedToWindow();
		virtual	void				Draw(BRect bounds);

		virtual	void				KeyDown(const char* bytes, int32 numBytes);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);

	private:
				rgb_color			ColorAt(BPoint where);
				void				Close();

				BBitmap*			fBitmap;
				uint32				fWorkspace;
				BWindow*			fHiddenWindow;
};


#endif	// _BACKGROUND_VIEW_H
