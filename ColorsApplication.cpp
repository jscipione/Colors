/*
 * Copyright 2009-2012 Haiku, Inc. All Rights Reserved.
 * Copyright 2001-2008 Werner Freytag.
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *		Werner Freytag <freytag@gmx.de>
 * Authors:
 *		John Scipione <jscipione@gmail.com>
 */


#include "ColorsApplication.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Alert.h>
#include <Catalog.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Path.h>

#include "ColorsWindow.h"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "ColorsApplication"


static const char* kSettingsFileName = "Colors!_settings";
static const char* kAppSig = "application/x-vnd.Haiku-Colors!";

static const float kDefaultWindowWidth  = 400.0;
static const float kDefaultWindowHeight = 300.0;


ColorsApplication::ColorsApplication()
	:
	BApplication(kAppSig),
	fColorsWindow(NULL)
{
}


ColorsApplication::~ColorsApplication()
{
}


void
ColorsApplication::ReadyToRun()
{
	_LoadSettings();

	BRect frame(0, 0, kDefaultWindowWidth - 1, kDefaultWindowHeight - 1);
	fColorsWindow = new ColorsWindow(frame);

	// reveal window
	fColorsWindow->Show();
}


void
ColorsApplication::AboutRequested()
{
	(new BAlert("About", B_TRANSLATE("Colors! 2.0\n\n"
		"©2001-2008 Werner Freytag\n"
		"©2009-2012 Haiku, Inc.\n"
		"Colors! icon by meanwhile\n\n"
		"History\n"
		"    2.0 (Apr 30, 2012) Layout with the Haiku layout APIs, update "
		"the icon, add more color containers, update the interface, "
		"improve the fore back color container control.\n"
		"    1.6 (Sep 11, 2001) Added the eye-dropper to pick a color from "
		"anywhere on the desktop. Added a popup menu with some useful window "
		"specific settings.\n"
		"    1.5.1 (Sep 5, 2001) Fixed some minor bugs, improved the text "
		"color container\n"
		"    1.5 (Sep 3, 2001) Added color containers, removed some minor "
		"bugs, improved speed.\n"
		"    1.0 (Aug 3, 2001) Initial release. Updates to come!"),
		B_TRANSLATE("Close")))->Go();
}


bool
ColorsApplication::QuitRequested()
{
	fColorsWindow->QuitRequested();
	_SaveSettings();

	return true;
}


// #pragma mark -


void
ColorsApplication::_LoadSettings()
{
	// locate preferences file
	BFile prefsFile;
	if (_InitSettingsFile(&prefsFile, false) < B_OK) {
		printf("no preference file found.\n");
		return;
	}

	// unflatten settings data
	if (fSettings.Unflatten(&prefsFile) < B_OK) {
		printf("error unflattening settings.\n");
	}
}


void
ColorsApplication::_SaveSettings()
{
	// flatten entire archive and write to settings file
	BFile prefsFile;
	status_t ret = _InitSettingsFile(&prefsFile, true);
	if (ret < B_OK) {
		fprintf(stderr, "ColorsApplication::_SaveSettings() - "
			"error creating file: %s\n", strerror(ret));
		return;
	}

	ret = fSettings.Flatten(&prefsFile);
	if (ret < B_OK) {
		fprintf(stderr, "ColorsApplication::_SaveSettings() - error flattening "
			"to file: %s\n", strerror(ret));
		return;
	}
}


status_t
ColorsApplication::_InitSettingsFile(BFile* file, bool write)
{
	// find user settings directory
	BPath prefsPath;
	status_t ret = find_directory(B_USER_SETTINGS_DIRECTORY, &prefsPath);
	if (ret < B_OK)
		return ret;

	ret = prefsPath.Append(kSettingsFileName);
	if (ret < B_OK)
		return ret;

	if (write) {
		ret = file->SetTo(prefsPath.Path(),
			B_CREATE_FILE | B_ERASE_FILE | B_WRITE_ONLY);
	} else
		ret = file->SetTo(prefsPath.Path(), B_READ_ONLY);

	return ret;
}


int
main()
{
	ColorsApplication* app = new ColorsApplication();
	app->Run();

	return 0;
}
