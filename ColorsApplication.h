/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 */
#ifndef _COLORS_APPLICATION_H
#define _COLORS_APPLICATION_H


#include <Application.h>


extern const char* kSignature;


class BFile;
class ColorsWindow;

class ColorsApplication : public BApplication {
	public:
									ColorsApplication();
		virtual						~ColorsApplication();
		virtual	void				ReadyToRun();
		virtual	void				AboutRequested();
		virtual	bool				QuitRequested();

				BMessage*			Settings() { return &fSettings; }

	private:
				void				_LoadSettings();
				void				_SaveSettings();
				status_t			_InitSettingsFile(BFile* file, bool write);

				ColorsWindow*		fColorsWindow;
				BMessage			fSettings;
};


#endif	// _COLORS_APPLICATION_H
