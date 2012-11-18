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


#include "ColorsWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Dragger.h>
#include <GroupLayout.h>
#include <MenuItem.h>
#include <Message.h>
#include <PopUpMenu.h>

#include "ColorsApplication.h"
#include "ColorPickerView.h"
#include "ColorWellsView.h"


ColorsWindow::ColorsWindow(BRect frame)
	:
	BWindow(frame, "Colors!", B_TITLED_WINDOW,
		B_QUIT_ON_WINDOW_CLOSE | B_ASYNCHRONOUS_CONTROLS
		| B_NOT_RESIZABLE | B_NOT_ZOOMABLE | B_AUTO_UPDATE_SIZE_LIMITS),
	fColorPickerView(new ColorPickerView()),
	fColorWellsView(new ColorWellsView())
{
	SetLayout(new BGroupLayout(B_VERTICAL, 0));

	GetLayout()->AddView(fColorPickerView);

	// create replicant dragger
	BRect replicantFrame(fColorPickerView->Frame());
	replicantFrame.top = replicantFrame.bottom - 7.0f;
	replicantFrame.left = replicantFrame.right - 7.0f;
	BDragger* dragger = new BDragger(replicantFrame, fColorPickerView,
		B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	fColorPickerView->AddChild(dragger);

	GetLayout()->AddView(fColorWellsView);

	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	BPoint where;
	if (settings->FindPoint("window_position", &where) < B_OK)
		where = BPoint(200.0, 100.0);

	MoveTo(where);

	bool windowFloating;
	if (settings->FindBool("window_floating", &windowFloating) == B_OK
		&& windowFloating) {
		SetFeel(B_FLOATING_ALL_WINDOW_FEEL);
	} else
		SetFeel(B_NORMAL_WINDOW_FEEL);

	bool windowAcceptsFirstClick;
	if (settings->FindBool("window_accept_first_click",
			&windowAcceptsFirstClick) == B_OK && windowAcceptsFirstClick) {
		SetFlags(Flags() | B_WILL_ACCEPT_FIRST_CLICK);
	} else
		SetFlags(Flags() & ~B_WILL_ACCEPT_FIRST_CLICK);

	bool windowAllWorkspaces;
	if (settings->FindBool("window_all_workspaces",
			&windowAllWorkspaces) == B_OK && windowAllWorkspaces) {
		SetWorkspaces(B_ALL_WORKSPACES);
	} else
		SetWorkspaces(B_CURRENT_WORKSPACE);
}


ColorsWindow::~ColorsWindow()
{
	fColorPickerView->SaveSettings(
		static_cast<ColorsApplication*>(be_app)->Settings());
	fColorWellsView->SaveSettings();

	BMessage* settings = static_cast<ColorsApplication*>(be_app)->Settings();

	settings->RemoveName("window_position");
	settings->AddPoint("window_position", Frame().LeftTop());

	settings->RemoveName("window_floating");
	settings->AddBool("window_floating",
		Feel() == B_FLOATING_ALL_WINDOW_FEEL);

	settings->RemoveName("window_accept_first_click");
	settings->AddBool("window_accept_first_click",
		Flags() & B_WILL_ACCEPT_FIRST_CLICK);

	settings->RemoveName("window_all_workspaces");
	settings->AddBool("window_all_workspaces",
		Workspaces() == B_ALL_WORKSPACES);
}


void
ColorsWindow::MessageReceived(BMessage *message)
{
	switch(message->what) {
		case MSG_POPUP_MENU:
		{
			BPoint where;
			if (message->FindPoint("where", &where) != B_OK)
				return;

			BPopUpMenu *menu = new BPopUpMenu("", false, false);
			BMenuItem *item[5];

			item[0] = new BMenuItem("Always on top", NULL);
			item[0]->SetMarked(Feel() == B_FLOATING_ALL_WINDOW_FEEL);
			menu->AddItem(item[0]);

			item[1] = new BMenuItem("Accept first mouse click", NULL);
			item[1]->SetMarked(Flags() & B_WILL_ACCEPT_FIRST_CLICK);
			menu->AddItem(item[1]);

			item[2] = new BMenuItem("All workspaces", NULL);
			item[2]->SetMarked(Workspaces() == B_ALL_WORKSPACES);
			menu->AddItem(item[2]);

			item[3] = new BSeparatorItem();
			menu->AddItem(item[3]);

			item[4] = new BMenuItem("About Colors!", NULL);
			menu->AddItem(item[4]);

			menu->ResizeToPreferred();

			BMenuItem *selected_item = menu->Go(where);

			if (selected_item == item[0]) {
				BMessage* settings
					= static_cast<ColorsApplication*>(be_app)->Settings();
				if (!settings->FindBool("floating_msg_showed")) {
					(new BAlert("Information",
						"Be aware of the following restrictions "
						"on the 'Always on top' mode:\n"
						"\n"
						"· You cannot minimize the window.\n"
						"· The window does not appear in Deskbar.\n"
						"\nUnfortunately there is no way to change this.",
						"Thanks for the information"))->Go();

					settings->AddBool("floating_msg_showed", true);
				}

				SetFeel(Feel() == B_FLOATING_ALL_WINDOW_FEEL
					? B_NORMAL_WINDOW_FEEL : B_FLOATING_ALL_WINDOW_FEEL);
			} else if (selected_item == item[1]) {
				SetFlags(Flags() ^ B_WILL_ACCEPT_FIRST_CLICK);
			} else if (selected_item == item[2]) {
				SetWorkspaces(Workspaces() == B_ALL_WORKSPACES
					? B_CURRENT_WORKSPACE : B_ALL_WORKSPACES);
			} else if (selected_item == item[4]) {
				static_cast<ColorsApplication*>(be_app)->AboutRequested();
			}

			delete menu;
			break;
		}

		default:
			BWindow::MessageReceived(message);
			break;
	}
}
