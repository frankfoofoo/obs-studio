/******************************************************************************
    Copyright (C) 2015 by Sean Nelson <audiohacked@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <stdio.h>
#include <util/dstr.h>
#include <util/darray.h>
#include <util/platform.h>
#include <obs-module.h>

#include "obs-nvenc.h"
#include "nvEncodeAPI.h"

#define SET_VER(configStruct, type) {configStruct.version = type##_VER;}

/* ------------------------------------------------------------------------- */
void clear_data(struct obs_nvenc *obsnv)
{
	if (obsnv->nvenc_encoder) {
		obsnv->api->nvEncDestroyEncoder(obsnv->nvenc_encoder);
		bfree(obsnv->nvenc_encoder);
		bfree(obsnv->sei);
		bfree(obsnv->extra_data);
		bfree(obsnv->api);

		obsnv->nvenc_encoder    = NULL;
		obsnv->sei              = NULL;
		obsnv->extra_data       = NULL;
		obsnv->api              = NULL;
	}
}

/* ------------------------------------------------------------------------- */
NVENCSTATUS obs_nvenc_helper_create_instance(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NVENCSTATUS (*nvEncodeAPICreateInstance)(NV_ENCODE_API_FUNCTION_LIST *);

	// get function from DLL
	nvEncodeAPICreateInstance = os_dlsym(obs_nvenc_lib,
			"NvEncodeAPICreateInstance");

	if (nvEncodeAPICreateInstance == NULL) {
		error("Unable to create API Instance");
	}

	// set up pointer for nVidia Encoder Function List
	memset(&obsnv->api, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
	obsnv->api->version = NV_ENCODE_API_FUNCTION_LIST_VER;

	// set up API Instance
	nvStatus = nvEncodeAPICreateInstance(obsnv->api);
	
	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_open_session(void* data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	// set up session parameter data structure
	memset(&obsnv->session_params, 0, sizeof(obsnv->session_params));
	obsnv->session_params.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;

	// fill parameters with data
	obsnv->session_params.device = gs_get_context();
	if (gs_get_device_type() == GS_DEVICE_DIRECT3D_11)
		obsnv->session_params.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	else
		obsnv->session_params.deviceType = NV_ENC_DEVICE_TYPE_CUDA;
	obsnv->session_params.reserved = NULL;
	obsnv->session_params.apiVersion = NVENCAPI_VERSION;
	
	// Open Session Encoder
	obsnv->api->nvEncOpenEncodeSessionEx(&obsnv->session_params,
		&obsnv->nvenc_encoder);

	return nvStatus;
}
GUID obs_nvenc_helper_get_guid_codec(void)
{
	return NV_ENC_CODEC_H264_GUID;
}

GUID obs_nvenc_helper_get_guid_preset(void)
{
	return NV_ENC_PRESET_LOW_LATENCY_HQ_GUID;
}

NVENCSTATUS obs_nvenc_helper_select_codec(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t codec_count = 0, preset_count = 0;
	obsnv->nvenc_codec = obs_nvenc_helper_get_guid_codec();

	//obsnv->api->nvEncGetEncodeGUIDCount(obsnv->nvenc_encoder, &codec_count);
	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_get_preset(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	obsnv->nvenc_preset = obs_nvenc_helper_get_guid_preset();
	//obsnv->api->nvEncGetEncodePresetCount(obsnv->nvenc_encoder, obsnv->nvenc_codec, &preset_count);
	//obsnv->api->nvEncGetEncodePresetGUIDs(obsnv->nvenc_encoder, obsnv->nvenc_codec, obsnv->nvenc_preset_array, preset_count, &preset_count);
	nvStatus = obsnv->api->nvEncGetEncodePresetConfig(obsnv->nvenc_encoder, obsnv->nvenc_codec, obsnv->nvenc_preset, obsnv->nvenc_config_preset);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_set_profile(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t profile_count = 0;

	obsnv->api->nvEncGetEncodeProfileGUIDCount(obsnv->nvenc_encoder, obsnv->nvenc_codec, &profile_count);
	obsnv->api->nvEncGetEncodeProfileGUIDs(obsnv->nvenc_encoder, obsnv->nvenc_codec, obsnv->nvenc_profile_array, profile_count, &profile_count);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_get_input_formats(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t input_format_count = 0;

	obsnv->api->nvEncGetInputFormatCount(obsnv->nvenc_encoder, obsnv->nvenc_codec, &input_format_count);
	obsnv->api->nvEncGetInputFormats(obsnv->nvenc_encoder, obsnv->nvenc_codec, obsnv->nvenc_config_input_format, input_format_count, &input_format_count);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_init_encoder(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	//obsnv->nvenc_config_encode->profileGUID = obsnv->nvenc_profile;
	obsnv->nvenc_config_init->encodeGUID = obs_nvenc_helper_get_guid_codec();
	obsnv->nvenc_config_init->encodeWidth = (int)obs_encoder_get_width(obsnv->encoder);
	obsnv->nvenc_config_init->encodeHeight = (int)obs_encoder_get_height(obsnv->encoder);
	//obsnv->nvenc_config_init->reportSliceOffsets = 
	//obsnv->nvenc_config_init->enableEncodeAsync = 
	//obsnv->nvenc_config_init->encodeConfig = obsnv->nvenc_config_encode;
	//obsnv->nvenc_config_init->encodeConfig->encodeCodecConfig = (NV_ENC_CODEC_CONFIG)obsnv->nvenc_config_h264;
	obsnv->api->nvEncInitializeEncoder(obsnv->encoder, obsnv->nvenc_config_init);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_create_buffer(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	obsnv->api->nvEncCreateInputBuffer(obsnv->nvenc_encoder, obsnv->nvenc_buffer_input);
	obsnv->api->nvEncCreateBitstreamBuffer(obsnv->nvenc_encoder, obsnv->nvenc_buffer_output);

	return nvStatus;
}