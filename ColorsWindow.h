/*
 * Copyright 2009-2013 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLORS_WINDOW_H
#define _COLORS_WINDOW_H


#include <Window.h>

#define	MSG_POPUP_MENU		'PopU'
#define	MSG_AUTOBORDERLESS	'AutB'


class ColorContainersView;
class ColorPickerView;

class ColorsWindow : public BWindow {
public:
									ColorsWindow(BRect frame);
	virtual							~ColorsWindow();

	virtual	void					MessageReceived(BMessage* message);

private:
			ColorPickerView*		fColorPickerView;
			ColorContainersView*	fColorContainersView;
};


#endif	// _COLORS_WINDOW_H
