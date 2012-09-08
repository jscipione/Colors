/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef WEB_SAFE_SELECTOR_H
#define WEB_SAFE_SELECTOR_H


#include <View.h>

#define MSG_WEBSAFE 'WebS'


class WebSafeSelector : public BView {
	public:
									WebSafeSelector(BRect frame);
		virtual						~WebSafeSelector();

		virtual	void				Draw(BRect updateRect);
		virtual	void				MouseDown(BPoint where);

				rgb_color			GetColor() const;
				void				SetColor(rgb_color color);
				void				SetColor(long int color);

	private:
				rgb_color			fColor;
};

#endif	// WEB_SAFE_SELECTOR_H
