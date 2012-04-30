/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_WELL_H
#define _COLOR_WELL_H


#include <View.h>

#define MSG_MESSAGERUNNER 'MsgR'


class BMessageRunner;

class ColorWell : public BView {
	public:
									ColorWell(BRect frame);
		virtual						~ColorWell();

		virtual	void				Draw(BRect updateRect);

		virtual	void				MessageReceived(BMessage *message);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				rgb_color			GetColor() const;
				void				SetColor(rgb_color color);
				void				SetColor(long int color);

	private:
				void				DragColor(BPoint where);

				bool				fMouseDown;
				BPoint				fMouseOffset;
				rgb_color			fColor;
				BMessageRunner*		fMessageRunner;
				bool				fGotFirstClick;
};


#endif	// _COLOR_WELL_H
