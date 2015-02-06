#include <obs-module.h>

#if defined (_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-nvenc", "en-US")

extern struct obs_encoder_info obs_nvenc_encoder;

bool obs_module_load(void)
{
	void *obs_nvenc_lib = NULL;
    void *nvEncodeAPICreateInstance;

#if defined (_WIN32)
	obs_nvenc_lib = LoadLibrary(TEXT("nvEncodeAPI.dll"));
#endif

#if defined (_WIN64)
	obs_nvenc_lib = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
#endif
#if defined (LINUX)
	obs_nvenc_lib = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
#endif

	if (obs_nvenc_lib == NULL) {
		return false;
	}

#if defined(_WIN32)
	nvEncodeAPICreateInstance = (void*)GetProcAddress(obs_nvenc_lib, "NvEncodeAPICreateInstance");
#else
	nvEncodeAPICreateInstance = (void*)dlsym(obs_nvenc_lib, "NvEncodeAPICreateInstance");
#endif

	obs_register_encoder(&obs_nvenc_encoder);
	return true;
}
