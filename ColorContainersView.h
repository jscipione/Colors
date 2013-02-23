/*
 * Copyright 2009-2013 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_CONTAINERS_VIEW_H
#define _COLOR_CONTAINERS_VIEW_H


#include <View.h>


static const int32 kColorContainerCount  = 32;


class ColorContainer;
class ForeBackSelector;

class ColorContainersView : public BView {
	public:
									ColorContainersView();
		virtual						~ColorContainersView();

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

				ColorContainer*		fColorContainer[kColorContainerCount];
				ForeBackSelector*	fForeBackSelector;
};


#endif	// _COLOR_CONTAINERS_VIEW_H
