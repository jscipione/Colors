/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef COLOR_SLIDER_H
#define COLOR_SLIDER_H


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

		virtual	void				FrameResized(float width, float height);

		virtual	void				MouseDown(BPoint where);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseUp(BPoint where);

				void				SetValue(int32 value);

				bool				IsTracking() const { return fMouseDown; }

				void				OtherValues(float* value1,
										float* value2) const;
				void				SetOtherValues(float value1,
										float value2);

				void				SetMarkerToColor(rgb_color color);
				void				SetModeAndValues(color_mode mode,
										float value1, float value2);

				void				Update(int depth);

	private:	
		static	inline	void		_DrawColorLine(BView *view, float y,
										int r, int g, int b);
				void				_TrackMouse(BPoint where);
		static	int32				_UpdateThread(void *data);

				color_mode			fColorMode;
				float				fFixedValue1;
				float				fFixedValue2;

				bool				fMouseDown;

				BBitmap*			fBgBitmap;
				BView*				fBgView;

				thread_id			fUpdateThread;
				port_id				fUpdatePort;
};


#endif	// COLOR_SLIDER_H
