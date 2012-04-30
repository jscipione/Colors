/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLOR_MODE_H
#define _COLOR_MODE_H


enum color_mode {
	R_SELECTED = 0x01,
	G_SELECTED = 0x02,
	B_SELECTED = 0x04,
	H_SELECTED = 0x08,
	S_SELECTED = 0x10,
	V_SELECTED = 0x20
};


#endif	// _COLOR_MODE_H
