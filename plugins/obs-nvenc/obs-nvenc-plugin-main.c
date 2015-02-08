#include <obs-module.h>
#include <util/platform.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-nvenc", "en-US")

extern struct obs_encoder_info obs_nvenc_encoder;
void *obs_nvenc_lib = NULL;

bool obs_module_load(void)
{

#if defined (_WIN64)
	obs_nvenc_lib = os_dlopen("nvEncodeAPI64.dll");
#endif

#if defined (_WIN32)
	obs_nvenc_lib = os_dlopen("nvEncodeAPI.dll");
#endif

#if defined (LINUX)
	obs_nvenc_lib = os_dlopen("libnvidia-encode.so");
#endif

	if (obs_nvenc_lib == NULL) {
		blog(LOG_ERROR, "[obs-nvenc] ERROR: nVidia Encoder DLL missing!");
		return true;
	}

	obs_register_encoder(&obs_nvenc_encoder);
	return true;
}
