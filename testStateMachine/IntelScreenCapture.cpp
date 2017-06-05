#include "stdafx.h"

// 根据是否使用Intel特性来决定是否使用Intel SDK
#ifdef INTEL_SDK

#include "IntelScreenCapture.h"

#pragma comment(lib, "libmfx.lib")

#include "mfxvideo.h"		/* The SDK include file */ 
#include "mfxvideo++.h"		/* Optional for C++ development */ 
#include "mfxplugin.h"		/* Plugin development */

CIntelScreenCapture::CIntelScreenCapture()
{

}

CIntelScreenCapture::~CIntelScreenCapture()
{

}

bool CIntelScreenCapture::Test()
{
	mfxSession session;
	mfxVersion ver;
	ver.Major = 1;
	ver.Minor = 17;
	MFXInit(MFX_IMPL_HARDWARE, &ver, &session);

	MFXVideoUSER_Load(session, &MFX_PLUGINID_CAPTURE_HW, ver.Version);
	mfxVideoParam *in = NULL;
	// input parameters structure 
	mfxVideoParam *out = NULL;
	// output parameters structure 
	/* allocate structures and fill input parameters structure, zero unused fields */
	MFXVideoDECODE_Query(session, in, out);

	mfxFrameAllocRequest request;


	MFXClose(session);

	return true;
}
	/* check supported parameters */ 
//	MFXVideoDECODE_QueryIOSurf(session, &in, &request);
//	allocate_pool_of_frame_surfaces(request.NumFrameSuggested);
//	MFXVideoDECODE_Init(session, &in);
//	sts = MFX_ERR_MORE_DATA;
//	for (;;) {
//		find_unlocked_surface_from_the_pool(&work);
//		sts = MFXVideoDECODE_DecodeFrameAsync(session, NULL, work, &disp, &syncp);
//		if (sts == MFX_ERR_MORE_SURFACE) continue;
//		… // other error handling  if (sts==MFX_ERR_NONE) {   MFXVideoCORE_SyncOperation(session, syncp, INFINITE);
//			do_something_with_decoded_frame(disp);
//	}
//} MFXVideoDECODE_Close(session);
//free_pool_of_frame_surfaces();
//MFXClose(session)
//}

bool CIntelScreenCapture::Init()
{
	return true;
}

#endif //INTEL_SDK