/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef WEB_SAFE_SELECTOR_H
#define WEB_SAFE_SELECTOR_H


#include "ColorSelector.h"


class WebSafeSelector : public ColorSelector {
	public:
									WebSafeSelector();
		virtual						~WebSafeSelector();

		virtual	void				Draw(BRect updateRect);

		virtual	void				SetColor(long int color);
		virtual	void				SetColor(rgb_color color);
};

#endif	// WEB_SAFE_SELECTOR_H
