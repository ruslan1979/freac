 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <dialogs/config/config_encoders.h>
#include <dialogs/config/configcomponent.h>

#include <dllinterfaces.h>
#include <utilities.h>
#include <boca.h>

using namespace BoCA::AS;

BonkEnc::ConfigureEncoders::ConfigureEncoders()
{
	Point	 pos;
	Size	 size;

	currentConfig = Config::Get();

	onTheFly	= currentConfig->enc_onTheFly;
	keepWaves	= currentConfig->enc_keepWaves;
	useInputDir	= currentConfig->writeToInputDir;
	allowOverwrite	= currentConfig->allowOverwrite;
	singleFile	= currentConfig->encodeToSingleFile;
	unicode_files	= currentConfig->useUnicodeNames;

	pos.x	= 7;
	pos.y	= 11;
	size.cx	= 344;
	size.cy	= 43;

	group_encoder	= new GroupBox(BonkEnc::i18n->TranslateString("Encoder"), pos, size);

	pos.x	= 17;
	pos.y	= 23;
	size.cx	= 186;
	size.cy	= 0;

	combo_encoder	= new ComboBox(pos, size);

	Registry	&boca = Registry::Get();

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_ENCODER) continue;

		combo_encoder->AddEntry(boca.GetComponentName(i));

		if (currentConfig->encoderID == boca.GetComponentID(i)) combo_encoder->SelectNthEntry(combo_encoder->Length() - 1);
	}

	pos.y	-= 1;
	pos.x	+= 194;
	size.cx	= 130;

	button_config	= new Button(BonkEnc::i18n->TranslateString("Configure encoder"), NIL, pos, size);
	button_config->onAction.Connect(&ConfigureEncoders::ConfigureEncoder, this);

	pos.x	= 7;
	pos.y	= 66;
	size.cx	= 344;
	size.cy	= 93;

	group_outdir	= new GroupBox(BonkEnc::i18n->TranslateString("Output directory"), pos, size);

	pos.x	+= 10;
	pos.y	+= 14;
	size.cx	= 236;
	size.cy	= 0;

	check_useInputDir	= new CheckBox(BonkEnc::i18n->TranslateString("Use input file directory if possible"), pos, size, &useInputDir);
	check_useInputDir->onAction.Connect(&ConfigureEncoders::ToggleUseInputDir, this);

	pos.x	+= 17;
	pos.y	+= 23;
	size.cx	= 219;
	size.cy	= 0;

	check_allowOverwrite	= new CheckBox(BonkEnc::i18n->TranslateString("Allow overwriting input file"), pos, size, &allowOverwrite);

	ToggleUseInputDir();

	pos.x	= 17;
	pos.y	+= 25;
	size.cx	= 236;
	size.cy	= 0;

	edit_outdir	= new EditBox(currentConfig->enc_outdir, pos, size, 0);

	pos.x	+= 244;
	pos.y	-= 1;
	size.cx	= 0;

	button_outdir_browse= new Button(BonkEnc::i18n->TranslateString("Browse"), NIL, pos, size);
	button_outdir_browse->onAction.Connect(&ConfigureEncoders::SelectDir, this);

	pos.x	= 7;
	pos.y	= 171;
	size.cx	= 344;
	size.cy	= 43;

	group_filename	= new GroupBox(BonkEnc::i18n->TranslateString("Filename pattern"), pos, size);

	pos.x	= 17;
	pos.y	+= 12;
	size.cx	= 324;
	size.cy	= 0;

	edit_filename	= new EditBox(currentConfig->enc_filePattern, pos, size, 0);

	list_filename	= new ListBox(pos, size);
	list_filename->AddEntry("<artist> - <title>");
	list_filename->AddEntry("<artist>\\<artist> - <title>");
	list_filename->AddEntry("<artist> - <album> - <track> - <title>");
	list_filename->AddEntry("<artist> - <album>\\<track> - <title>");
	list_filename->AddEntry("<artist> - <album>\\<artist> - <album> - <track> - <title>");
	list_filename->AddEntry("<track> - <artist> - <title>");
	list_filename->AddEntry("<album>\\<track> - <artist> - <title>");
	list_filename->AddEntry("<genre>\\<artist> - <title>");
	list_filename->AddEntry("<filename>");

	edit_filename->SetDropDownList(list_filename);

	pos.x	= 359;
	pos.y	= 11;
	size.cx	= 178;
	size.cy	= 94;

	group_options		= new GroupBox(BonkEnc::i18n->TranslateString("Options"), pos, size);

	pos.x	+= 10;
	pos.y	+= 14;
	size.cx	= 157;
	size.cy	= 0;

	check_onTheFly		= new CheckBox(BonkEnc::i18n->TranslateString("Encode \'On-The-Fly\'"), pos, size, &onTheFly);
	check_onTheFly->onAction.Connect(&ConfigureEncoders::ToggleOnTheFly, this);

	pos.y += 26;

	check_keepWaves		= new CheckBox(BonkEnc::i18n->TranslateString("Keep ripped wave files"), pos, size, &keepWaves);

	ToggleOnTheFly();

	pos.y += 26;

	check_singleFile	= new CheckBox(BonkEnc::i18n->TranslateString("Encode to single file"), pos, size, &singleFile);
	check_singleFile->onAction.Connect(&ConfigureEncoders::ToggleEncodeToSingleFile, this);

	ToggleEncodeToSingleFile();

	pos.x	= 359;
	pos.y	= 171;
	size.cx	= 178;
	size.cy	= 43;

	group_unicode		= new GroupBox(BonkEnc::i18n->TranslateString("Unicode"), pos, size);

	pos.x	+= 10;
	pos.y	+= 14;
	size.cx	= 157;
	size.cy	= 0;

	check_unicode_files	= new CheckBox(BonkEnc::i18n->TranslateString("Use Unicode filenames"), pos, size, &unicode_files);

	if (!Setup::enableUnicode) check_unicode_files->Deactivate();

	Add(group_encoder);
	Add(combo_encoder);
	Add(button_config);
	Add(group_outdir);
	Add(check_useInputDir);
	Add(check_allowOverwrite);
	Add(edit_outdir);
	Add(button_outdir_browse);
	Add(group_filename);
	Add(edit_filename);
	Add(group_options);
	Add(check_onTheFly);
	Add(check_keepWaves);
	Add(check_singleFile);
	Add(group_unicode);
	Add(check_unicode_files);

	SetSize(Size(544, 250));
}

BonkEnc::ConfigureEncoders::~ConfigureEncoders()
{
	DeleteObject(group_encoder);
	DeleteObject(combo_encoder);
	DeleteObject(button_config);
	DeleteObject(group_outdir);
	DeleteObject(check_useInputDir);
	DeleteObject(check_allowOverwrite);
	DeleteObject(edit_outdir);
	DeleteObject(button_outdir_browse);
	DeleteObject(group_filename);
	DeleteObject(edit_filename);
	DeleteObject(list_filename);
	DeleteObject(group_options);
	DeleteObject(check_onTheFly);
	DeleteObject(check_keepWaves);
	DeleteObject(check_singleFile);
	DeleteObject(group_unicode);
	DeleteObject(check_unicode_files);
}

Void BonkEnc::ConfigureEncoders::SelectDir()
{
	DirSelection	*dialog = new DirSelection();

	dialog->SetParentWindow(GetContainerWindow());
	dialog->SetCaption(String("\n").Append(BonkEnc::i18n->TranslateString("Select the folder in which the encoded files will be placed:")));
	dialog->SetDirName(String(edit_outdir->GetText()).Replace("<installdrive>", Utilities::GetInstallDrive()));

	if (dialog->ShowDialog() == Success())
	{
		edit_outdir->SetText(dialog->GetDirName());
	}

	DeleteObject(dialog);
}

Void BonkEnc::ConfigureEncoders::ConfigureEncoder()
{
	Registry	&boca = Registry::Get();
	String		 encoderID;

	for (Int i = 0, n = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_ENCODER) continue;

		if (n++ == combo_encoder->GetSelectedEntryNumber())
		{
			encoderID = boca.GetComponentID(i);

			break;
		}
	}

	Component	*component = boca.CreateComponentByID(encoderID);
	ConfigLayer	*layer = component->GetConfigurationLayer();

	if (layer != NIL)
	{
		ConfigComponentDialog	*dlg = new ConfigComponentDialog(layer);

		dlg->ShowDialog();

		DeleteObject(dlg);

		onChangeEncoderSettings.Emit(encoderID);
	}
	else
	{
		QuickMessage(BonkEnc::i18n->TranslateString("No options can be configured for the WAVE Out filter!"), BonkEnc::i18n->TranslateString("WAVE Out filter"), MB_OK, IDI_INFORMATION);
	}

	component->FreeConfigurationLayer();

	boca.DeleteComponent(component);
}

Void BonkEnc::ConfigureEncoders::ToggleOnTheFly()
{
	if (onTheFly)	check_keepWaves->Deactivate();
	else		check_keepWaves->Activate();
}

Void BonkEnc::ConfigureEncoders::ToggleUseInputDir()
{
	if (useInputDir) check_allowOverwrite->Activate();
	else		 check_allowOverwrite->Deactivate();
}

Void BonkEnc::ConfigureEncoders::ToggleEncodeToSingleFile()
{
	if (singleFile)	{ onTheFly = True; check_onTheFly->Deactivate(); }
	else		{ check_onTheFly->Activate(); }

	ToggleOnTheFly();
}

Int BonkEnc::ConfigureEncoders::SaveSettings()
{
	Directory	 outputDirectory(String(edit_outdir->GetText()).Replace("<installdrive>", Utilities::GetInstallDrive()));

	if ((Setup::enableUnicode ? SetCurrentDirectoryW(String(outputDirectory)) : SetCurrentDirectoryA(String(outputDirectory))) == False)
	{
		Int	 selection = QuickMessage(BonkEnc::i18n->TranslateString("The output directory does not exist! Do you want to create it?"), BonkEnc::i18n->TranslateString("Error"), MB_YESNOCANCEL, IDI_QUESTION);

		if	(selection == IDYES)	outputDirectory.Create();
		else if (selection == IDCANCEL)	return Error();
	}

	if (Setup::enableUnicode)	SetCurrentDirectoryW(Application::GetApplicationDirectory());
	else				SetCurrentDirectoryA(Application::GetApplicationDirectory());

	Registry	&boca = Registry::Get();

	for (Int i = 0, n = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != BoCA::COMPONENT_TYPE_ENCODER) continue;

		if (n++ == combo_encoder->GetSelectedEntryNumber())
		{
			currentConfig->encoderID = boca.GetComponentID(i);

			break;
		}
	}

	currentConfig->enc_outdir		= edit_outdir->GetText();
	currentConfig->enc_filePattern		= edit_filename->GetText();
	currentConfig->enc_onTheFly		= onTheFly;
	currentConfig->writeToInputDir		= useInputDir;
	currentConfig->allowOverwrite		= allowOverwrite;
	currentConfig->encodeToSingleFile	= singleFile;
	currentConfig->enc_keepWaves		= keepWaves;
	currentConfig->useUnicodeNames		= unicode_files;

	if (currentConfig->enc_outdir[currentConfig->enc_outdir.Length() - 1] != '\\') currentConfig->enc_outdir.Append("\\");

	return Success();
}