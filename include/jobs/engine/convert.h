 /* fre:ac - free audio converter
  * Copyright (C) 2001-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_FREAC_JOB_CONVERT
#define H_FREAC_JOB_CONVERT

#include <boca.h>

#include "../job.h"
#include "../../engine/converter.h"

namespace freac
{
	class JobConvert : public Job
	{
		private:
			static Bool					 conversionRunning;
			static Bool					 conversionPaused;

			static Bool					 skipTrack;
			static Bool					 stopConversion;

			Array<BoCA::Track>				 tracks;
			Array<Int>					 trackActions;

			String						 singleOutFile;
			BoCA::Format					 singleTrackSampleFormat;

			Void						 UpdateProgress(Int, Int);

			BoCA::Format					 GetSingleTrackSampleFormat() const;
			BoCA::Track					 ConsolidateTrackInfo();
		public:
			static Bool					 IsConverting()	{ return conversionRunning; }
			static Bool					 IsPaused()	{ return conversionPaused;  }

			static Void					 Pause()	{ if (conversionRunning) conversionPaused = True;  }
			static Void					 Resume()	{ if (conversionRunning) conversionPaused = False; }

			static Void					 Skip()		{ if (conversionRunning) skipTrack = True; }
			static Void					 Stop();

									 JobConvert(const Array<BoCA::Track> &);
			virtual						~JobConvert();

			virtual Error					 Precheck();
			virtual Bool					 ReadyToRun();
		signals:
			static Signal0<Void>				 onStartEncoding;
			static Signal1<Void, Bool>			 onFinishEncoding;

			static Signal3<Void, const BoCA::Track &,
					     const String &,
					     ConversionStep>		 onEncodeTrack;
			static Signal0<Void>				 onFinishTrack;

			static Signal2<Void, Int, Int>			 onTrackProgress;
			static Signal2<Void, Int, Int>			 onTotalProgress;
		slots:
			Void						 OnWorkerReportError(const String &);
			Void						 OnWorkerReportWarning(const String &);

			virtual Error					 Perform();
	};
};

#endif
