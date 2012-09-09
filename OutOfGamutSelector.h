/*
 * Copyright 2012 Haiku, Inc. All Rights Reserved.
 * Distributed under the terms of the MIT License.
 */
#ifndef OUT_OF_GAMUT_SELECTOR_H
#define OUT_OF_GAMUT_SELECTOR_H


#include "ColorSelector.h"


class OutOfGamutSelector : public ColorSelector {
	public:
									OutOfGamutSelector();
		virtual						~OutOfGamutSelector();

		virtual	void				Draw(BRect updateRect);

		virtual	void				SetColor(rgb_color color);
};

#endif	// OUT_OF_GAMUT_SELECTOR_H
