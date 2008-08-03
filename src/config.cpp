 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <config.h>
#include <bonkenc.h>

BonkEnc::Config *BonkEnc::Config::instance = NIL;

BonkEnc::Config::Config()
{
	languageChanged		= False;
	deleteAfterEncoding	= False;
	shutdownAfterEncoding	= False;

	cdrip_autoRead_active	= False;
	cdrip_timeout		= 0;

	saveSettingsOnExit	= True;

	maxActiveJobs		= 2;

	LoadSettings();
}

BonkEnc::Config::~Config()
{
	if (saveSettingsOnExit) SaveSettings();
}

BonkEnc::Config *BonkEnc::Config::Get()
{
	if (instance == NIL)
	{
		instance = new Config();
	}

	return instance;
}

Void BonkEnc::Config::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}

Bool BonkEnc::Config::CanChangeConfig()
{
	if (BonkEnc::Get()->encoder->IsEncoding())
	{
		Utilities::ErrorMessage("Cannot change settings while encoding!");

		return False;
	}

	return True;
}

Void BonkEnc::Config::SetSaveSettingsOnExit(Bool nSaveSettingsOnExit)
{
	saveSettingsOnExit = nSaveSettingsOnExit;
}

Bool BonkEnc::Config::LoadSettings()
{
	String		 personalDir = S::System::System::GetPersonalFilesDirectory();
	String		 programsDir = S::System::System::GetProgramFilesDirectory();

	if (Application::GetApplicationDirectory().ToUpper().StartsWith(programsDir.ToUpper()))
	{
		configDir = S::System::System::GetApplicationDataDirectory();

		if (configDir != "") configDir.Append("BonkEnc\\");

		Directory(configDir).Create();
	}
	else
	{
		configDir = Application::GetApplicationDirectory();
	}

	Configuration	*config = new Configuration(String(configDir).Append("config.xml"), False);

	firstStart				= config->GetIntValue("Settings", "FirstStart", 1);
	language				= config->GetStringValue("Settings", "Language", "");
	encoderID				= config->GetStringValue("Settings", "Encoder", "wave-out");
	enc_outdir				= config->GetStringValue("Settings", "EncoderOutDir", personalDir);
	enc_filePattern				= config->GetStringValue("Settings", "EncoderFilenamePattern", "<artist> - <title>");
	enc_onTheFly				= config->GetIntValue("Settings", "EncodeOnTheFly", 1);
	enc_keepWaves				= config->GetIntValue("Settings", "KeepWaveFiles", 0);
	playlist_useEncOutdir			= config->GetIntValue("Settings", "PlaylistUseEncOutDir", 1);
	playlist_outdir				= config->GetStringValue("Settings", "PlaylistOutDir", personalDir);
	playlist_filePattern			= config->GetStringValue("Settings", "PlaylistFilenamePattern", "<artist> - <album>");
	useUnicodeNames				= config->GetIntValue("Settings", "UseUnicodeFilenames", 1);
	showTitleInfo				= config->GetIntValue("Settings", "ShowTitleInfo", 1);
	showTooltips				= config->GetIntValue("Settings", "ShowTooltips", 1);
	wndPos.x				= config->GetIntValue("Settings", "WindowPosX", 100);
	wndPos.y				= config->GetIntValue("Settings", "WindowPosY", 100);
	wndSize.cx				= config->GetIntValue("Settings", "WindowSizeX", 650);
	wndSize.cy				= config->GetIntValue("Settings", "WindowSizeY", 474);
	maximized				= config->GetIntValue("Settings", "WindowMaximized", 0);
	tab_width_track				= config->GetIntValue("Settings", "TabWidthTrack", 50);
	tab_width_length			= config->GetIntValue("Settings", "TabWidthLength", 80);
	tab_width_size				= config->GetIntValue("Settings", "TabWidthSize", 80);
	showTips				= config->GetIntValue("Settings", "ShowTips", 1);
	tipOffset				= config->GetIntValue("Settings", "TipOffset", 0);
	checkUpdatesAtStartup			= config->GetIntValue("Settings", "CheckUpdatesAtStartup", 1);
	createPlaylist				= config->GetIntValue("Settings", "CreatePlaylist", 0);
	createCueSheet				= config->GetIntValue("Settings", "CreateCueSheet", 0);
	lastAddedDir				= config->GetStringValue("Settings", "LastAddedDir", "");
	lastAddedPattern			= config->GetStringValue("Settings", "LastAddedPattern", "");
	writeToInputDir				= config->GetIntValue("Settings", "WriteToInputDirectory", 0);
	allowOverwrite				= config->GetIntValue("Settings", "AllowOverwriteSource", 0);
	encodeToSingleFile			= config->GetIntValue("Settings", "EncodeToSingleFile", 0);

	enable_id3v1				= config->GetIntValue("Settings", "EnableID3V1", 0);
	enable_id3v2				= config->GetIntValue("Settings", "EnableID3V2", 1);
	enable_vctags				= config->GetIntValue("Settings", "EnableVorbisCommentTags", 1);
	enable_mp4meta				= config->GetIntValue("Settings", "EnableMP4Metadata", 1);
	id3v1_encoding				= config->GetStringValue("Settings", "ID3V1Encoding", "ISO-8859-1");
	id3v2_encoding				= config->GetStringValue("Settings", "ID3V2Encoding", "UTF-16LE");
	vctag_encoding				= config->GetStringValue("Settings", "VorbisCommentTagEncoding", "UTF-8");
	mp4meta_encoding			= config->GetStringValue("Settings", "MP4MetadataEncoding", "UTF-8");
	default_comment				= config->GetStringValue("Settings", "DefaultComment", String("BonkEnc ").Append(BonkEnc::shortVersion).Append(" <http://www.bonkenc.org/>"));
	copy_picture_tags			= config->GetIntValue("Settings", "CopyPictureTags", 1);

	cdrip_activedrive			= config->GetIntValue("CDRip", "ActiveCDROM", 0);
	cdrip_debuglevel			= config->GetIntValue("CDRip", "DebugCDRip", 0);
	cdrip_paranoia				= config->GetIntValue("CDRip", "CDParanoia", 0);
	cdrip_paranoia_mode			= config->GetIntValue("CDRip", "CDParanoiaMode", 3);
	cdrip_detectJitterErrors		= config->GetIntValue("CDRip", "DetectJitterErrors", 1);
	cdrip_detectC2Errors			= config->GetIntValue("CDRip", "DetectC2Errors", 1);
	cdrip_jitter				= config->GetIntValue("CDRip", "JitterCorrection", 0);
	cdrip_swapchannels			= config->GetIntValue("CDRip", "SwapChannels", 0);
	cdrip_locktray				= config->GetIntValue("CDRip", "LockTray", 1);
	cdrip_read_cdtext			= config->GetIntValue("CDRip", "ReadCDText", 1);
	cdrip_read_cdplayerini			= config->GetIntValue("CDRip", "ReadCDPlayerIni", 1);
	cdrip_ntscsi				= config->GetIntValue("CDRip", "UseNTSCSI", 1);
	cdrip_autoRead				= config->GetIntValue("CDRip", "AutoReadContents", 1);
	cdrip_autoRip				= config->GetIntValue("CDRip", "AutoRip", 0);
	cdrip_autoEject				= config->GetIntValue("CDRip", "EjectAfterRipping", 0);
	cdrip_numdrives				= 0;

	enable_auto_cddb			= config->GetIntValue("freedb", "AutoCDDBQueries", 0);
	enable_overwrite_cdtext			= config->GetIntValue("freedb", "OverwriteCDText", 1);
	enable_cddb_cache			= config->GetIntValue("freedb", "EnableCDDBCache", 1);
	enable_local_cddb			= config->GetIntValue("freedb", "EnableLocalCDDB", 0);
	freedb_dir				= config->GetStringValue("freedb", "Directory", "freedb\\");
	enable_remote_cddb			= config->GetIntValue("freedb", "EnableRemoteCDDB", 1);
	freedb_server				= config->GetStringValue("freedb", "Server", "freedb.freedb.org");
	freedb_mode				= config->GetIntValue("freedb", "Mode", 0);
	freedb_cddbp_port			= config->GetIntValue("freedb", "CDDBPPort", 8880);
	freedb_http_port			= 80;
	freedb_query_path			= config->GetStringValue("freedb", "QueryPath", "/~cddb/cddb.cgi");
	freedb_submit_path			= config->GetStringValue("freedb", "SubmitPath", "/~cddb/submit.cgi");
	freedb_email				= config->GetStringValue("freedb", "eMail", "cddb@bonkenc.org");
	freedb_proxy_mode			= config->GetIntValue("freedb", "ProxyMode", 0);
	freedb_proxy				= config->GetStringValue("freedb", "Proxy", "localhost");
	freedb_proxy_port			= config->GetIntValue("freedb", "ProxyPort", 1080);
	freedb_proxy_user			= config->GetStringValue("freedb", "ProxyUserName", "");
	freedb_proxy_password			= config->GetStringValue("freedb", "ProxyPassword", "");
	update_joblist				= config->GetIntValue("freedb", "UpdateJoblistOnSubmit", 1);

	delete config;

	if (enc_outdir[enc_outdir.Length() - 1] != '\\') enc_outdir.Append("\\");
	if (playlist_outdir[playlist_outdir.Length() - 1] != '\\') playlist_outdir.Append("\\");
	if (freedb_dir[freedb_dir.Length() - 1] != '\\') freedb_dir.Append("\\");

	if (encodeToSingleFile && !enc_onTheFly) enc_onTheFly = True;

	return True;
}

Bool BonkEnc::Config::SaveSettings()
{
	Configuration	*config = new Configuration();
	Bool		 retVal = True;
	String		 str;

	if (config->Open(String(configDir).Append("config.xml"), True) == Success())
	{
		config->SetIntValue("Settings", "FirstStart", 0);
		config->SetStringValue("Settings", "Language", language);
		config->SetStringValue("Settings", "Encoder", encoderID);
		config->SetStringValue("Settings", "EncoderOutDir", enc_outdir);
		config->SetStringValue("Settings", "EncoderFilenamePattern", enc_filePattern);
		config->SetIntValue("Settings", "EncodeOnTheFly", enc_onTheFly);
		config->SetIntValue("Settings", "KeepWaveFiles", enc_keepWaves);
		config->SetIntValue("Settings", "PlaylistUseEncOutDir", playlist_useEncOutdir);
		config->SetStringValue("Settings", "PlaylistOutDir", playlist_outdir);
		config->SetStringValue("Settings", "PlaylistFilenamePattern", playlist_filePattern);
		config->SetIntValue("Settings", "UseUnicodeFilenames", useUnicodeNames);
		config->SetIntValue("Settings", "ShowTitleInfo", showTitleInfo);
		config->SetIntValue("Settings", "ShowTooltips", showTooltips);
		config->SetIntValue("Settings", "WindowPosX", wndPos.x);
		config->SetIntValue("Settings", "WindowPosY", wndPos.y);
		config->SetIntValue("Settings", "WindowSizeX", wndSize.cx);
		config->SetIntValue("Settings", "WindowSizeY", wndSize.cy);
		config->SetIntValue("Settings", "WindowMaximized", maximized);
		config->SetIntValue("Settings", "TabWidthTrack", tab_width_track);
		config->SetIntValue("Settings", "TabWidthLength", tab_width_length);
		config->SetIntValue("Settings", "TabWidthSize", tab_width_size);
		config->SetIntValue("Settings", "ShowTips", showTips);
		config->SetIntValue("Settings", "TipOffset", tipOffset);
		config->SetIntValue("Settings", "CheckUpdatesAtStartup", checkUpdatesAtStartup);
		config->SetIntValue("Settings", "CreatePlaylist", createPlaylist);
		config->SetIntValue("Settings", "CreateCueSheet", createCueSheet);
		config->SetStringValue("Settings", "LastAddedDir", lastAddedDir);
		config->SetStringValue("Settings", "LastAddedPattern", lastAddedPattern);
		config->SetIntValue("Settings", "WriteToInputDirectory", writeToInputDir);
		config->SetIntValue("Settings", "AllowOverwriteSource", allowOverwrite);
		config->SetIntValue("Settings", "EncodeToSingleFile", encodeToSingleFile);

		config->SetIntValue("Settings", "EnableID3V1", enable_id3v1);
		config->SetIntValue("Settings", "EnableID3V2", enable_id3v2);
		config->SetIntValue("Settings", "EnableVorbisCommentTags", enable_vctags);
		config->SetIntValue("Settings", "EnableMP4Metadata", enable_mp4meta);
		config->SetStringValue("Settings", "ID3V1Encoding", id3v1_encoding);
		config->SetStringValue("Settings", "ID3V2Encoding", id3v2_encoding);
		config->SetStringValue("Settings", "VorbisCommentTagEncoding", vctag_encoding);
		config->SetStringValue("Settings", "MP4MetadataEncoding", mp4meta_encoding);
		config->SetStringValue("Settings", "DefaultComment", default_comment);
		config->SetIntValue("Settings", "CopyPictureTags", copy_picture_tags);

		config->SetIntValue("CDRip", "ActiveCDROM", cdrip_activedrive);
		config->SetIntValue("CDRip", "DebugCDRip", cdrip_debuglevel);
		config->SetIntValue("CDRip", "CDParanoia", cdrip_paranoia);
		config->SetIntValue("CDRip", "CDParanoiaMode", cdrip_paranoia_mode);
		config->SetIntValue("CDRip", "DetectJitterErrors", cdrip_detectJitterErrors);
		config->SetIntValue("CDRip", "DetectC2Errors", cdrip_detectC2Errors);
		config->SetIntValue("CDRip", "JitterCorrection", cdrip_jitter);
		config->SetIntValue("CDRip", "SwapChannels", cdrip_swapchannels);
		config->SetIntValue("CDRip", "LockTray", cdrip_locktray);
		config->SetIntValue("CDRip", "ReadCDText", cdrip_read_cdtext);
		config->SetIntValue("CDRip", "ReadCDPlayerIni", cdrip_read_cdplayerini);
		config->SetIntValue("CDRip", "UseNTSCSI", cdrip_ntscsi);
	 	config->SetIntValue("CDRip", "AutoReadContents", cdrip_autoRead);
	 	config->SetIntValue("CDRip", "AutoRip", cdrip_autoRip);
		config->SetIntValue("CDRip", "EjectAfterRipping", cdrip_autoEject);

		config->SetIntValue("freedb", "AutoCDDBQueries", enable_auto_cddb);
		config->SetIntValue("freedb", "OverwriteCDText", enable_overwrite_cdtext);
		config->SetIntValue("freedb", "EnableCDDBCache", enable_cddb_cache);
		config->SetIntValue("freedb", "EnableLocalCDDB", enable_local_cddb);
		config->SetStringValue("freedb", "Directory", freedb_dir);
		config->SetIntValue("freedb", "EnableRemoteCDDB", enable_remote_cddb);
		config->SetStringValue("freedb", "Server", freedb_server);
		config->SetIntValue("freedb", "Mode", freedb_mode);
		config->SetIntValue("freedb", "CDDBPPort", freedb_cddbp_port);
		config->SetStringValue("freedb", "QueryPath", freedb_query_path);
		config->SetStringValue("freedb", "SubmitPath", freedb_submit_path);
		config->SetStringValue("freedb", "eMail", freedb_email);
		config->SetIntValue("freedb", "ProxyMode", freedb_proxy_mode);
		config->SetStringValue("freedb", "Proxy", freedb_proxy);
		config->SetIntValue("freedb", "ProxyPort", freedb_proxy_port);
		config->SetStringValue("freedb", "ProxyUserName", freedb_proxy_user);
		config->SetStringValue("freedb", "ProxyPassword", freedb_proxy_password);
		config->SetIntValue("freedb", "UpdateJoblistOnSubmit", update_joblist);

		config->Save();
	}
	else
	{
		retVal = False;
	}

	delete config;

	return retVal;
}
