/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_FIELD_H
#define _COLOR_FIELD_H


#include <Control.h>

#include "color_mode.h"

#define MSG_COLOR_FIELD 'ColF'
#define MSG_UPDATE		'Updt'


class BBitmap;
class BPoint;

class ColorField : public BControl {

public:
									ColorField(color_mode mode,
										float fixed_value);
		virtual						~ColorField();

		virtual	void				Draw(BRect updateRect);
		virtual	status_t			Invoke(BMessage* message = NULL);
		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				void				PositionMarkerAt(BPoint where);

				void				SetModeAndValue(color_mode mode,
										float fixed_value);
				void				SetFixedValue(float fixed_value);
				void				SetMarkerToColor(rgb_color color);

				void				Update(int depth);

private:
	inline		void				DrawColorPoint(BPoint pt, int r, int g,
										int b);

		static	int32				UpdateThread(void *data);

				color_mode			fColorMode;
				float				fFixedValue;

				BPoint				fMarkerPosition;
				BPoint				fLastMarkerPosition;

				bool				fMouseDown;

				BBitmap*			fBgBitmap[2];
				BView*				fBgView[2];

				thread_id			fUpdateThread;
				port_id				fUpdatePort;
};


#endif // _COLOR_FIELD_H
