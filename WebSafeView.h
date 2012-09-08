/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef WEB_SAFE_VIEW_H
#define WEB_SAFE_VIEW_H


#include <View.h>

#define MSG_WEBSAFE 'WebS'


class WebSafeView : public BView {
	public:
									WebSafeView(BRect frame);
		virtual						~WebSafeView();

		virtual	void				Draw(BRect updateRect);
		virtual	void				MessageReceived(BMessage *message);
		virtual	void				MouseDown(BPoint where);

				rgb_color			GetColor() const;
				void				SetColor(rgb_color color);
				void				SetColor(long int color);

	private:
				rgb_color			fColor;
};

#endif	// WEB_SAFE_VIEW_H
