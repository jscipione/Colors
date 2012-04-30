/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _EYE_DROPPER_H
#define _EYE_DROPPER_H


#include <Control.h>

#define MSG_EYEDROPPER	'EyeD'


class BBitmap;

class EyeDropper : public BControl {
	public:
									EyeDropper();
		virtual						~EyeDropper();
			
		virtual	void				Draw(BRect updateRect);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

	private:
				BBitmap*			FetchImage(const char* imgname);

				bool				fMouseDown;
				bool				fButtonDown;
				BBitmap*			fImage[2];
};


#endif	// _EYE_DROPPER_H
