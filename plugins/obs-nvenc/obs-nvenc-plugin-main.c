#ifdef _WIN32 || _WIN64
	#include <windows.h>
#else
	#include <dlfcn.h>
#endif

#include <obs-module.h>

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("obs-nvenc", "en-US")

extern struct obs_encoder_info obs_nvenc_encoder;

bool obs_module_load(void)
{
	void *obs_nvenc_lib = NULL;
    void *nvEncodeAPICreateInstance;

#if defined (NV_WINDOWS)
	#if defined (_WIN64)
		obs_nvenc_lib = LoadLibrary(TEXT("nvEncodeAPI64.dll"));
	#else
		obs_nvenc_lib = LoadLibrary(TEXT("nvEncodeAPI.dll"));
	#endif
#else
	obs_nvenc_lib = dlopen("libnvidia-encode.so.1", RTLD_LAZY);
#endif

	if (obs_nvenc_lib == NULL) {
		return false;
	}

#if defined(NV_WINDOWS)
    nvEncodeAPICreateInstance = (void*)GetProcAddress(m_hinstLib, "NvEncodeAPICreateInstance");
#else
    nvEncodeAPICreateInstance = (void*)dlsym(m_hinstLib, "NvEncodeAPICreateInstance");
#endif

	obs_register_encoder(&obs_nvenc_encoder);
	return true;
}
