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

/* ------------------------------------------------------------------------- */

void clear_data(struct obs_nvenc *obsnv)
{
	if (obsnv->nvenc_device) {
		obsnv->api->nvEncDestroyInputBuffer(obsnv->nvenc_device, &obsnv->nvenc_buffer_input);
		obsnv->api->nvEncDestroyBitstreamBuffer(obsnv->nvenc_device, &obsnv->nvenc_buffer_output);
		obsnv->api->nvEncDestroyEncoder(obsnv->nvenc_device);

		bfree(obsnv->nvenc_device);
		bfree(obsnv->sei);
		bfree(obsnv->extra_data);
		bfree(obsnv->api);

		obsnv->nvenc_device    = NULL;
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
	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS session_params;

	// set up session parameter data structure
	memset(&session_params, 0, sizeof(session_params));
	session_params.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;

	// fill parameters with data
	session_params.device = gs_get_context();
	if (gs_get_device_type() == GS_DEVICE_DIRECT3D_11)
		session_params.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	else
		session_params.deviceType = NV_ENC_DEVICE_TYPE_CUDA;
	session_params.reserved = NULL;
	session_params.apiVersion = NVENCAPI_VERSION;
	
	// Open Session Encoder
	obsnv->api->nvEncOpenEncodeSessionEx(&session_params, &obsnv->nvenc_device);

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

	//obsnv->api->nvEncGetEncodeGUIDCount(obsnv->nvenc_device, &codec_count);
	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_get_preset(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t preset_count = 0;
//	GUID *nvenc_preset_array;
//	GUID *nvenc_config_preset;
	obsnv->nvenc_preset = obs_nvenc_helper_get_guid_preset();

//	obsnv->api->nvEncGetEncodePresetCount(obsnv->nvenc_device, obsnv->nvenc_codec, &preset_count);
//	memset(nvenc_preset_array, 0, preset_count*sizeof(GUID));

//	obsnv->api->nvEncGetEncodePresetGUIDs(obsnv->nvenc_device, obsnv->nvenc_codec, nvenc_preset_array, preset_count, &preset_count);

//	nvStatus = obsnv->api->nvEncGetEncodePresetConfig(obsnv->nvenc_device, obsnv->nvenc_codec, obsnv->nvenc_preset, nvenc_config_preset);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_set_profile(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t profile_count = 0;
	GUID *nvenc_profile_array;

	obsnv->api->nvEncGetEncodeProfileGUIDCount(obsnv->nvenc_device, obsnv->nvenc_codec, &profile_count);
	memset(&nvenc_profile_array, 0, profile_count*sizeof(GUID));
	obsnv->api->nvEncGetEncodeProfileGUIDs(obsnv->nvenc_device, obsnv->nvenc_codec, nvenc_profile_array, profile_count, &profile_count);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_get_input_formats(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t input_format_count = 0;
	NV_ENC_BUFFER_FORMAT *nvenc_config_input_format;

	obsnv->api->nvEncGetInputFormatCount(obsnv->nvenc_device, obsnv->nvenc_codec, &input_format_count);
	memset(&nvenc_config_input_format, 0, input_format_count*sizeof(NV_ENC_BUFFER_FORMAT));
	obsnv->api->nvEncGetInputFormats(obsnv->nvenc_device, obsnv->nvenc_codec, nvenc_config_input_format, input_format_count, &input_format_count);

	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_init_encoder(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_INITIALIZE_PARAMS nvenc_config_init;
	//obsnv->nvenc_config_encode->profileGUID = obsnv->nvenc_profile;
	memset(&nvenc_config_init, 0, sizeof(NV_ENC_INITIALIZE_PARAMS));
	SET_VER(nvenc_config_init, NV_ENC_INITIALIZE_PARAMS);
	nvenc_config_init.encodeGUID = obs_nvenc_helper_get_guid_codec();
	nvenc_config_init.encodeWidth = (int)obs_encoder_get_width(obsnv->encoder);
	nvenc_config_init.encodeHeight = (int)obs_encoder_get_height(obsnv->encoder);
	//obsnv->nvenc_config_init->reportSliceOffsets = 
	//obsnv->nvenc_config_init->enableEncodeAsync = 
	//obsnv->nvenc_config_init->encodeConfig = obsnv->nvenc_config_encode;
	//obsnv->nvenc_config_init->encodeConfig->encodeCodecConfig = (NV_ENC_CODEC_CONFIG)obsnv->nvenc_config_h264;
	obsnv->api->nvEncInitializeEncoder(obsnv->encoder, &nvenc_config_init);
	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_create_buffer(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	// initialize input buffer
	memset(&obsnv->nvenc_buffer_input, 0, sizeof(NV_ENC_CREATE_INPUT_BUFFER));
	SET_VER(obsnv->nvenc_buffer_input, NV_ENC_CREATE_INPUT_BUFFER);
	obsnv->nvenc_buffer_input.bufferFmt = NV_ENC_BUFFER_FORMAT_NV12_PL; // FIX ME
	obsnv->api->nvEncCreateInputBuffer(obsnv->nvenc_device, &obsnv->nvenc_buffer_input);
	
	// initialize output bitstream buffer
	memset(&obsnv->nvenc_buffer_output, 0, sizeof(NV_ENC_CREATE_BITSTREAM_BUFFER));
	SET_VER(obsnv->nvenc_buffer_output, NV_ENC_CREATE_BITSTREAM_BUFFER);
	obsnv->api->nvEncCreateBitstreamBuffer(obsnv->nvenc_device, &obsnv->nvenc_buffer_output);

	return nvStatus;
}

void obs_nvenc_helper_fill_frame(struct obs_nvenc *obsnv, struct encoder_frame *frame)
{
	video_t *video = obs_encoder_video(obsnv->encoder);
	const struct video_output_info *vid_info = video_output_get_info(video);

	obsnv->nvenc_picture.inputWidth = (int)obs_encoder_get_width(obsnv->encoder);
	obsnv->nvenc_picture.inputHeight = (int)obs_encoder_get_height(obsnv->encoder);
	obsnv->nvenc_picture.inputPitch = obsnv->nvenc_picture.inputWidth;
	//obsnv->nvenc_picture.encodePicFlags = ;
	//obsnv->nvenc_picture.frameIdx = ;
	obsnv->nvenc_picture.inputTimeStamp = frame->pts;
	//obsnv->nvenc_picture.inputDuration = ;
	obsnv->nvenc_picture.inputBuffer = frame->data[0];

	if (vid_info->format == VIDEO_FORMAT_I420) {
		obsnv->nvenc_picture.bufferFmt = NV_ENC_BUFFER_FORMAT_IYUV_PL;
	}
	else if (vid_info->format == VIDEO_FORMAT_NV12) {
		obsnv->nvenc_picture.bufferFmt = NV_ENC_BUFFER_FORMAT_NV12_PL;
	}
	else {
		obsnv->nvenc_picture.bufferFmt = NV_ENC_BUFFER_FORMAT_UNDEFINED;
	}

}

void obs_nvenc_helper_save_bitstream(struct obs_nvenc *obsnv, struct encoder_packet *packet)
{
	packet->data = obsnv->nvenc_buffer_output.bitstreamBuffer;
	packet->size = obsnv->nvenc_buffer_output.size;
	packet->type = OBS_ENCODER_VIDEO;
	//packet->pts = obsnv->nvenc_buffer_output.bitstreamBuffer
	//packet->dts = obsnv->nvenc_buffer_output
	//packet->keyframe = obsnv->nvenc_buffer_output
}