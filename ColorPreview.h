/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_PREVIEW_H
#define _COLOR_PREVIEW_H


#include <Control.h>

#define	MSG_COLOR_PREVIEW		'ColP'
#define	MSG_MESSAGERUNNER		'MsgR'


class BMessageRunner;

class ColorPreview : public BControl {
	public:

									ColorPreview();
		virtual						~ColorPreview();

		virtual	void				Draw(BRect updateRect);
		virtual	status_t			Invoke(BMessage* message = NULL);
		virtual	void				MessageReceived(BMessage* message);
		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				// changes the current color
				void				SetColor(rgb_color color);
				// also changes the old color
				void				SetNewColor(rgb_color color);

	private:
				void				_DragColor(BPoint where);

				rgb_color			fColor;
				rgb_color			fOldColor;

				bool				fMouseDown;
				BMessageRunner*		fMessageRunner;
};


#endif	// _COLOR_PREVIEW_H
