/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _FORE_BACK_SELECTOR_H
#define _FORE_BACK_SELECTOR_H


#include <View.h>


class BMessageRunner;
class ColorWell;

class ForeBackSelector : public BView {
	public:
									ForeBackSelector(BRect frame);
		virtual						~ForeBackSelector();

		virtual	void				Draw(BRect updateRect);
		virtual	void				DrawAfterChildren(BRect updateRect);

		virtual	void				MessageReceived(BMessage *message);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				ColorWell*			ForeColorWell() { return fForeColorWell; };
				ColorWell*			BackColorWell() { return fBackColorWell; };

	private:
				ColorWell*			fForeColorWell;
				ColorWell*			fBackColorWell;
};


#endif	// _FORE_BACK_SELECTOR_H
