/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef WEB_SAFE_SELECTOR_H
#define WEB_SAFE_SELECTOR_H


#include "ColorSelector.h"

#define MSG_WEBSAFE 'WebS'


class WebSafeSelector : public ColorSelector {
	public:
		virtual	void				Draw(BRect updateRect);
		virtual	void				SetColor(rgb_color color);

	private:
				rgb_color			fColor;
};

#endif	// WEB_SAFE_SELECTOR_H
