 /* BonkEnc version 0.9
  * Copyright (C) 2001-2003 Robert Kausch <robert.kausch@gmx.net>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <vorbisconfig.h>
#include <resources.h>

configureVorbisEnc::configureVorbisEnc(bonkEncConfig *config)
{
	Point	 pos;
	Size	 size;

	currentConfig = config;

	quality = currentConfig->vorbis_quality;
	abr = currentConfig->vorbis_bitrate;
	mode = currentConfig->vorbis_mode;

	mainWnd			= new Window(String("Ogg Vorbis ").Append(currentConfig->i18n->TranslateString("encoder configuration")));
	mainWnd_titlebar	= new Titlebar(false, false, true);
	divbar			= new Divider(42, OR_HORZ | OR_BOTTOM);
	mainWnd_layer_vbr	= new Layer();
	mainWnd_layer_abr	= new Layer();

	pos.x = 175;
	pos.y = 29;
	size.cx = 0;
	size.cy = 0;

	btn_cancel		= new Button(currentConfig->i18n->TranslateString("Cancel"), NIL, pos, size, Proc(mainWnd->*(&Window::Close)), mainWnd);
	btn_cancel->SetOrientation(OR_LOWERRIGHT);

	pos.x -= 88;

	btn_ok			= new Button(currentConfig->i18n->TranslateString("OK"), NIL, pos, size, Proc(&configureVorbisEnc::OK), this);
	btn_ok->SetOrientation(OR_LOWERRIGHT);

	pos.x = 7;
	pos.y = 11;
	size.cx = 344;
	size.cy = 43;

	group_mode		= new GroupBox(currentConfig->i18n->TranslateString("Encoding Mode"), pos, size);

	size.cy = 84;

	group_mode2		= new GroupBox(currentConfig->i18n->TranslateString("Encoding Mode"), pos, size);

	pos.x = 17;
	pos.y = 24;
	size.cx = 157;
	size.cy = 0;

	option_mode_vbr		= new OptionBox(String("VBR (").Append(currentConfig->i18n->TranslateString("Variable Bitrate")).Append(")"), pos, size, &mode, 0, Proc(&configureVorbisEnc::SetMode), this);

	pos.x += 166;

	option_mode_abr		= new OptionBox(String("ABR (").Append(currentConfig->i18n->TranslateString("Average Bitrate")).Append(")"), pos, size, &mode, 1, Proc(&configureVorbisEnc::SetMode), this);

	pos.x = 19;
	pos.y += 43;

	text_quality		= new Text(currentConfig->i18n->TranslateString("Quality").Append(":"), pos);

	pos.x += (text_quality->GetObjectProperties()->textSize.cx + 8);
	pos.y -= 2;
	size.cx = 283 - text_quality->GetObjectProperties()->textSize.cx;

	slider_quality		= new Slider(pos, size, OR_HORZ, &quality, 0, 100, Proc(&configureVorbisEnc::SetQuality), this);

	pos.x += (size.cx + 7);
	pos.y += 2;

	text_quality_value	= new Text("", pos);

	pos.x = 19;

	text_abr		= new Text(currentConfig->i18n->TranslateString("Average Bitrate").Append(":"), pos);

	pos.x += (text_abr->GetObjectProperties()->textSize.cx + 8);
	pos.y -= 2;
	size.cx = 248 - text_abr->GetObjectProperties()->textSize.cx;

	slider_abr		= new Slider(pos, size, OR_HORZ, &abr, 32, 512, Proc(&configureVorbisEnc::SetBitrate), this);

	pos.x += (size.cx + 8);
	pos.y -= 1;
	size.cx = 25;

	edit_abr		= new EditBox("", pos, size, EDB_NUMERIC, 3, Proc(&configureVorbisEnc::SetBitrateByEditBox), this);

	pos.x += 32;
	pos.y += 3;

	text_abr_kbps		= new Text("kbps", pos);

	SetQuality();
	SetBitrate();

	RegisterObject(mainWnd);

	mainWnd->RegisterObject(btn_ok);
	mainWnd->RegisterObject(btn_cancel);
	mainWnd->RegisterObject(group_mode);
	mainWnd->RegisterObject(group_mode2);
	mainWnd->RegisterObject(option_mode_vbr);
	mainWnd->RegisterObject(option_mode_abr);

	mainWnd_layer_vbr->RegisterObject(slider_quality);
	mainWnd_layer_vbr->RegisterObject(text_quality);
	mainWnd_layer_vbr->RegisterObject(text_quality_value);

	mainWnd_layer_abr->RegisterObject(slider_abr);
	mainWnd_layer_abr->RegisterObject(text_abr);
	mainWnd_layer_abr->RegisterObject(edit_abr);
	mainWnd_layer_abr->RegisterObject(text_abr_kbps);

	mainWnd->RegisterObject(mainWnd_titlebar);
	mainWnd->RegisterObject(mainWnd_layer_vbr);
	mainWnd->RegisterObject(mainWnd_layer_abr);
	mainWnd->RegisterObject(divbar);

	mainWnd->SetExStyle(WS_EX_TOOLWINDOW);
	mainWnd->SetIcon(SMOOTH::LoadImage("bonkenc.pci", 0, NIL));
	mainWnd->SetApplicationIcon(IDI_ICON);
	mainWnd->SetMetrics(Point(140, 140), Size(364, 173));

	switch (mode)
	{
		case 0:
			mainWnd_layer_abr->Hide();
			break;
		case 1:
			mainWnd_layer_vbr->Hide();
			break;
	}
}

configureVorbisEnc::~configureVorbisEnc()
{
	mainWnd->UnregisterObject(mainWnd_titlebar);
	mainWnd->UnregisterObject(mainWnd_layer_abr);
	mainWnd->UnregisterObject(mainWnd_layer_vbr);
	mainWnd->UnregisterObject(divbar);

	mainWnd->UnregisterObject(btn_ok);
	mainWnd->UnregisterObject(btn_cancel);
	mainWnd->UnregisterObject(group_mode);
	mainWnd->UnregisterObject(group_mode2);
	mainWnd->UnregisterObject(option_mode_abr);
	mainWnd->UnregisterObject(option_mode_vbr);

	mainWnd_layer_vbr->UnregisterObject(slider_quality);
	mainWnd_layer_vbr->UnregisterObject(text_quality);
	mainWnd_layer_vbr->UnregisterObject(text_quality_value);

	mainWnd_layer_abr->UnregisterObject(slider_abr);
	mainWnd_layer_abr->UnregisterObject(text_abr);
	mainWnd_layer_abr->UnregisterObject(edit_abr);
	mainWnd_layer_abr->UnregisterObject(text_abr_kbps);

	UnregisterObject(mainWnd);

	delete mainWnd_titlebar;
	delete mainWnd_layer_abr;
	delete mainWnd_layer_vbr;
	delete mainWnd;
	delete divbar;

	delete group_mode;
	delete group_mode2;
	delete option_mode_abr;
	delete option_mode_vbr;
	delete btn_ok;
	delete btn_cancel;

	delete slider_quality;
	delete text_quality;
	delete text_quality_value;

	delete slider_abr;
	delete text_abr;
	delete edit_abr;
	delete text_abr_kbps;
}

Int configureVorbisEnc::ShowDialog()
{
	mainWnd->Stay();

	return mainWnd->value;
}

Void configureVorbisEnc::OK()
{
	currentConfig->vorbis_mode = mode;
	currentConfig->vorbis_quality = quality;
	currentConfig->vorbis_bitrate = abr;

	mainWnd->Close();
}

Void configureVorbisEnc::SetMode()
{
	switch (mode)
	{
		case 0:
			mainWnd_layer_abr->Hide();
			mainWnd_layer_vbr->Show();
			break;
		case 1:
			mainWnd_layer_vbr->Hide();
			mainWnd_layer_abr->Show();
			break;
	}
}

Void configureVorbisEnc::SetQuality()
{
	String	 txt = String::DoubleToString(((double) quality) / 100);

	if (txt.Length() == 1)		txt.Append(".00");
	else if (txt.Length() == 3)	txt.Append("0");

	text_quality_value->SetText(txt);
}

Void configureVorbisEnc::SetBitrate()
{
	edit_abr->SetText(String::IntToString(abr));
}

Void configureVorbisEnc::SetBitrateByEditBox()
{
	slider_abr->SetValue(edit_abr->GetText().ToInt());
}
