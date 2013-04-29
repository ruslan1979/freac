 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/overwrite.h>

#include <config.h>
#include <resources.h>

BonkEnc::DialogConfirmOverwrite::DialogConfirmOverwrite(const Array<BoCA::Track> &tracks)
{
	userAction = ConfirmOverwrite::Action::None;

	BoCA::I18n	*i18n = BoCA::I18n::Get();

	i18n->SetContext("Dialogs::Overwrite");

	Rect	 workArea = S::System::MultiMonitor::GetActiveMonitorWorkArea();

	mainWnd			= new Window(i18n->TranslateString("Confirm overwrite"), workArea.GetPosition() + Point((workArea.GetSize().cx - 565) / 2, (workArea.GetSize().cy - 375) / 2), Size(565, 375));
	mainWnd->SetMinimumSize(Size(400, 220));
	mainWnd->SetRightToLeft(i18n->IsActiveLanguageRightToLeft());
	mainWnd->GetMainLayer()->onChangeSize.Connect(&DialogConfirmOverwrite::OnChangeSize, this);

	mainWnd_titlebar	= new Titlebar(TB_CLOSEBUTTON);
	divbar			= new Divider(39, OR_HORZ | OR_BOTTOM);

	button_cancel		= new Button(i18n->TranslateString("Cancel"), NIL, Point(87, 29), Size());
	button_cancel->onAction.Connect(&DialogConfirmOverwrite::OnCancel, this);
	button_cancel->SetOrientation(OR_LOWERRIGHT);

	button_skip		= new Button(i18n->TranslateString("Skip all"), NIL, Point(175, 29), Size());
	button_skip->onAction.Connect(&DialogConfirmOverwrite::OnSkip, this);
	button_skip->SetOrientation(OR_LOWERRIGHT);
	button_skip->SetWidth(Math::Max(80, button_skip->GetUnscaledTextWidth() + 14));
	button_skip->SetX(95 + button_skip->GetWidth());

	button_overwrite	= new Button(i18n->TranslateString("Confirm"), NIL, Point(183 + button_skip->GetWidth(), 29), Size());
	button_overwrite->onAction.Connect(&DialogConfirmOverwrite::OnOverwrite, this);
	button_overwrite->SetOrientation(OR_LOWERRIGHT);
	button_overwrite->SetWidth(Math::Max(80, button_overwrite->GetUnscaledTextWidth() + 14));
	button_overwrite->SetX(103 + button_skip->GetWidth() + button_overwrite->GetWidth());

	text_files		= new Text(String(i18n->TranslateString("The following %1 files already exist and will be overwritten")).Replace("%1", String::FromInt(tracks.Length())).Append(":"), Point(7, 5));

	Config	*bonkEncConfig	= Config::Get();

	i18n->SetContext("Joblist");

	button_select_all	= new Button(NIL, ImageLoader::Load(String(bonkEncConfig->resourcesPath).Append("freac.pci:18")), Point(-2, 20), Size(21, 21));
	button_select_all->onAction.Connect(&DialogConfirmOverwrite::SelectAll, this);
	button_select_all->SetFlags(BF_NOFRAME);
	button_select_all->SetTooltipText(i18n->TranslateString("Select all"));

	button_select_none	= new Button(NIL, ImageLoader::Load(String(bonkEncConfig->resourcesPath).Append("freac.pci:19")), Point(-2, 34), Size(21, 21));
	button_select_none->onAction.Connect(&DialogConfirmOverwrite::SelectNone, this);
	button_select_none->SetFlags(BF_NOFRAME);
	button_select_none->SetTooltipText(i18n->TranslateString("Select none"));

	button_select_toggle	= new Button(NIL, ImageLoader::Load(String(bonkEncConfig->resourcesPath).Append("freac.pci:20")), Point(-2, 48), Size(21, 21));
	button_select_toggle->onAction.Connect(&DialogConfirmOverwrite::ToggleSelection, this);
	button_select_toggle->SetFlags(BF_NOFRAME);
	button_select_toggle->SetTooltipText(i18n->TranslateString("Toggle selection"));

	i18n->SetContext("Dialogs::Overwrite");

	list_files		= new ListBox(Point(17, 24), Size(378, 90));
	list_files->SetFlags(LF_MULTICHECKBOX);
	list_files->onMarkEntry.Connect(&DialogConfirmOverwrite::OnMarkEntry, this);
	list_files->AddTab(i18n->TranslateString("File name"));
	list_files->AddTab(i18n->TranslateString("Action"), Math::Max(75, Math::Max(list_files->GetFont().GetUnscaledTextSizeX(i18n->TranslateString("Overwrite")), list_files->GetFont().GetUnscaledTextSizeX(i18n->TranslateString("Skip"))) + 25));

	foreach (const BoCA::Track &track, tracks)
	{
		list_files->AddEntry(String(track.outfile).Append("\t").Append(i18n->TranslateString("Overwrite")))->SetMark(True);
	}

	Add(mainWnd);

	mainWnd->Add(button_skip);
	mainWnd->Add(button_overwrite);
	mainWnd->Add(button_cancel);

	mainWnd->Add(text_files);

	mainWnd->Add(button_select_all);
	mainWnd->Add(button_select_none);
	mainWnd->Add(button_select_toggle);

	mainWnd->Add(list_files);

	mainWnd->Add(mainWnd_titlebar);
	mainWnd->Add(divbar);

	mainWnd->SetFlags(WF_NORMAL);
	mainWnd->SetIcon(ImageLoader::Load(String(Config::Get()->resourcesPath).Append("icons/freac.png")));
}

BonkEnc::DialogConfirmOverwrite::~DialogConfirmOverwrite()
{
	DeleteObject(mainWnd_titlebar);
	DeleteObject(mainWnd);
	DeleteObject(divbar);

	DeleteObject(text_files);

	DeleteObject(button_select_all);
	DeleteObject(button_select_none);
	DeleteObject(button_select_toggle);

	DeleteObject(list_files);

	DeleteObject(button_skip);
	DeleteObject(button_overwrite);
	DeleteObject(button_cancel);
}

const Error &BonkEnc::DialogConfirmOverwrite::ShowDialog()
{
	mainWnd->Stay();

	return error;
}

Int BonkEnc::DialogConfirmOverwrite::GetUserAction()
{
	return userAction;
}

const Array<Int> &BonkEnc::DialogConfirmOverwrite::GetUserActions()
{
	static Array<Int>	 userActions;

	userActions.RemoveAll();

	for (Int i = 0; i < list_files->Length(); i++)
	{
		if	(GetUserAction() == ConfirmOverwrite::Action::Confirm) userActions.Add(list_files->GetNthEntry(i)->IsMarked() ? ConfirmOverwrite::Action::Overwrite : ConfirmOverwrite::Action::Skip);
		else if (GetUserAction() == ConfirmOverwrite::Action::SkipAll) userActions.Add(ConfirmOverwrite::Action::Skip);
		else							       userActions.Add(ConfirmOverwrite::Action::None);
	}

	return userActions;
}

Void BonkEnc::DialogConfirmOverwrite::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(mainWnd->GetMainLayer()->GetPosition(), mainWnd->GetMainLayer()->GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_files->SetSize(clientSize - Size(24, 71));
}

Void BonkEnc::DialogConfirmOverwrite::SelectAll()
{
	for (Int i = 0; i < list_files->Length(); i++)
	{
		if (!list_files->GetNthEntry(i)->IsMarked()) list_files->GetNthEntry(i)->SetMark(True);
	}
}

Void BonkEnc::DialogConfirmOverwrite::SelectNone()
{
	for (Int i = 0; i < list_files->Length(); i++)
	{
		if (list_files->GetNthEntry(i)->IsMarked()) list_files->GetNthEntry(i)->SetMark(False);
	}
}

Void BonkEnc::DialogConfirmOverwrite::ToggleSelection()
{
	for (Int i = 0; i < list_files->Length(); i++)
	{
		if (list_files->GetNthEntry(i)->IsMarked()) list_files->GetNthEntry(i)->SetMark(False);
		else					    list_files->GetNthEntry(i)->SetMark(True);
	}
}

Void BonkEnc::DialogConfirmOverwrite::OnMarkEntry(ListEntry *entry)
{
	BoCA::I18n	*i18n = BoCA::I18n::Get();

	i18n->SetContext("Dialogs::Overwrite");

	String		 file	= entry->GetText().Head(entry->GetText().FindLast("\t"));
	String		 action = i18n->TranslateString("Overwrite");

	if (!entry->IsMarked()) action = i18n->TranslateString("Skip");

	entry->SetText(String(file).Append("\t").Append(action));
}

Void BonkEnc::DialogConfirmOverwrite::OnSkip()
{
	userAction = ConfirmOverwrite::Action::SkipAll;

	mainWnd->Close();
}

Void BonkEnc::DialogConfirmOverwrite::OnOverwrite()
{
	userAction = ConfirmOverwrite::Action::Confirm;

	mainWnd->Close();
}

Void BonkEnc::DialogConfirmOverwrite::OnCancel()
{
	userAction = ConfirmOverwrite::Action::Cancel;

	mainWnd->Close();
}