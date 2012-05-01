/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_SLIDER_H
#define _COLOR_SLIDER_H


#include <Control.h>

#include "color_mode.h"

#define MSG_UPDATE			'Updt'
#define MSG_COLOR_SLIDER	'ColS'


class BBitmap;

class ColorSlider : public BControl {
	public:
									ColorSlider(color_mode mode,
										float value1, float value2);
		virtual						~ColorSlider();

		virtual	void				AttachedToWindow();
		virtual	void				Draw(BRect updateRect);
		virtual	status_t			Invoke(BMessage* message = NULL);
		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				void				Update(int depth);
		inline	void				DrawColorLine(float y, int r, int g,
										int b);

				void				SetModeAndValues(color_mode mode,
										float value1, float value2);
				void				SetOtherValues(float value1, float value2);

				void				SetMarkerToColor(rgb_color color);
				void				SetValue(int32 value);

	private:
		static	int32				UpdateThread(void *data);
		static	inline	void		DrawColorLine(BView *view, float y,
										int r, int g, int b);

				color_mode			fColorMode;
				float				fFixedValue1;
				float				fFixedValue2;

				bool				fMouseDown;

				BBitmap*			fBgBitmap;
				BView*				fBgView;

				thread_id			fUpdateThread;
				port_id				fUpdatePort;
};


#endif	// _COLOR_SLIDER_H
