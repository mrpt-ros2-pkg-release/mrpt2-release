/* +------------------------------------------------------------------------+
   |                     Mobile Robot Programming Toolkit (MRPT)            |
   |                          https://www.mrpt.org/                         |
   |                                                                        |
   | Copyright (c) 2005-2019, Individual contributors, see AUTHORS file     |
   | See: https://www.mrpt.org/Authors - All rights reserved.               |
   | Released under BSD License. See: https://www.mrpt.org/License          |
   +------------------------------------------------------------------------+ */

#include "vision-precomp.h"  // Precompiled headers

#include <mrpt/vision/CFeatureExtraction.h>

// Universal include for all versions of OpenCV
#include <mrpt/otherlibs/do_opencv_includes.h>

using namespace mrpt;
using namespace mrpt::vision;
using namespace mrpt::img;
using namespace mrpt::system;
using namespace std;

void CFeatureExtraction::internal_computeLogPolarImageDescriptors(
	const mrpt::img::CImage& in_img, CFeatureList& in_features)
{
	MRPT_START
#if MRPT_HAS_OPENCV

	mrpt::system::CTimeLoggerEntry tle(
		profiler, "internal_computeLogPolarImageDescriptors");

	ASSERT_(options.LogPolarImagesOptions.radius > 1);
	ASSERT_(options.LogPolarImagesOptions.num_angles > 1);
	ASSERT_(options.LogPolarImagesOptions.rho_scale > 0);

	const unsigned int radius = options.LogPolarImagesOptions.radius;
	const unsigned int patch_h = options.LogPolarImagesOptions.num_angles;
	const double rho_scale = options.LogPolarImagesOptions.rho_scale;
	const unsigned int patch_w =
		rho_scale * std::log(static_cast<double>(radius));

	mrpt::img::CImage logpolar_frame(
		patch_w, patch_h, in_img.getChannelCount());

	// Compute intensity-domain spin images
	for (auto& f : in_features)
	{
		// Overwrite scale with the descriptor scale:
		f.keypoint.octave = radius;

		// Use OpenCV to convert:
#if MRPT_OPENCV_VERSION_NUM < 0x300
		const cv::Mat& in = in_img.asCvMatRef();
		cv::Mat& out = logpolar_frame.asCvMatRef();
		cvLogPolar(
			&in, &out,
#else
		cv::logPolar(
			in_img.asCvMatRef(), logpolar_frame.asCvMatRef(),
#endif
			cv::Point2f(f.keypoint.pt.x, f.keypoint.pt.y), rho_scale,
			CV_INTER_LINEAR + CV_WARP_FILL_OUTLIERS);

		// Get the image as a matrix and save as patch:
		f.descriptors.LogPolarImg.emplace();
		logpolar_frame.getAsMatrix(*f.descriptors.LogPolarImg);

	}  // end for it

#else
	THROW_EXCEPTION("This method needs MRPT compiled with OpenCV support");
#endif
	MRPT_END
}
