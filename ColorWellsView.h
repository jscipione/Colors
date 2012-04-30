/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_WELLS_VIEW_H
#define _COLOR_WELLS_VIEW_H


#include <View.h>


static const int32 kColorWellCount  = 32;

class ColorWell;
class ForeBackSelector;

class ColorWellsView : public BView {
	public:
									ColorWellsView();
		virtual						~ColorWellsView();

		virtual	void				AttachedToWindow();
		virtual	void				Draw(BRect updateRect);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage *message);
		virtual	void				MouseUp(BPoint where);

				void				SaveSettings();
	private:
				bool				fMouseDown;
				BPoint				fMouseOffset;

				ColorWell*			fColorWell[kColorWellCount];
				ForeBackSelector*	fForeBackSelector;
};


#endif	// _COLOR_WELLS_VIEW_H
