/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef COLOR_SELECTOR_H
#define COLOR_SELECTOR_H


#include <Control.h>

#define MSG_COLOR_SELECTOR 'CSel'

#define INDICATOR_RECT BRect(4.0, 0.0, 16.0, 12.0)
#define COLOR_RECT BRect(0.0, 16.0, 18.0, 34.0)


class ColorSelector : public BControl {
	public:
									ColorSelector();
		virtual						~ColorSelector();

		virtual	void				Draw(BRect updateRect);
		virtual	status_t			Invoke(BMessage* mesesage = NULL);
		virtual	void				MouseMoved(BPoint where, uint32 code,
										const BMessage* message);
		virtual	void				MouseDown(BPoint where);

		virtual	void				Hide();
		virtual void				Show();
				bool				IsHidden();

		virtual	rgb_color			GetColor() const;
		virtual	void				SetColor(long int color);
		virtual	void				SetColor(rgb_color color);

	protected:
				rgb_color			fColor;
				bool				fIsHidden;
				bool				fMouseOver;
};

#endif	// COLOR_SELECTOR_H
