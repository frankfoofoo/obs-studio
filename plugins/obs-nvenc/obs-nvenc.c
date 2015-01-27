/******************************************************************************
    Copyright (C) 2014 by Hugh Bailey <obs.jim@gmail.com>

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

#include <nvEncodeAPI.h>
#include <nvUtils.h>

#ifndef _STDINT_H_INCLUDED
#define _STDINT_H_INCLUDED
#endif

#define do_log(level, format, ...) \
	blog(level, "[nvenc encoder: '%s'] " format, \
			obs_encoder_get_name(obsx264->encoder), ##__VA_ARGS__)

#define warn(format, ...)  do_log(LOG_WARNING, format, ##__VA_ARGS__)
#define info(format, ...)  do_log(LOG_INFO,    format, ##__VA_ARGS__)
#define debug(format, ...) do_log(LOG_DEBUG,   format, ##__VA_ARGS__)

/* ------------------------------------------------------------------------- */

struct obs_nvenc {
	obs_encoder_t          *encoder;

	// x264_param_t           params;
	NVENCPI                *context;

	DARRAY(uint8_t)        packet_data;

	uint8_t                *extra_data;
	uint8_t                *sei;

	size_t                 extra_data_size;
	size_t                 sei_size;

	os_performance_token_t *performance_token;
};

/* ------------------------------------------------------------------------- */

static const char *obs_nvenc_getname(void)
{
	return "NVENC";
}

static void *obs_nvenc_create(obs_data_t *settings, obs_encoder_t *encoder)
{
	/*
	Open Encode Session with NvEncOpenEncodeSession
		Give License key: 
				NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS::clientKeyPtr
		Initialize the Encode Device with
				NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS::device
				NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS::deviceType = hasDX ?
					NV_ENC_DEVICE_TYPE_DIRECTX :
					NV_ENC_DEVICE_TYPE_CUDA;

	Select codec by
			getting codec count with NvEncGetEncodeGUIDCount
			allocate buffer for all GUIDs
			populate buffer with all GUIDs using NvEncGetEncodeGUIDs
	
	Query Capabilities Values
			NV_ENC_CAPS_PARAM::capsToQuery
			NvEncGetEncoderCaps

	Get Preset Encoder Configurations
	Select Encoder Profile
	Get Supported Input Format List
	Initialize HW Encoder Session
		Configure Encode Session Attributes
		Finalize the Codec Configuration for Encoding
		Setting Encode Session Attributes
	Create Resources to hold I/O Data
	
	*/
}

struct obs_encoder_info obs_nvenc_encoder = {
	.id             = "obs_nvenc",
	.type           = OBS_ENCODER_VIDEO,
	.codec          = "h264",
	.get_name       = obs_nvenc_getname,
	.create         = obs_nvenc_create,
	.destroy        = obs_nvenc_destroy,
	.encode         = obs_nvenc_encode,
	.update         = NULL,
	.get_properties = NULL,
	.get_defaults   = NULL,
	.get_extra_data = NULL,
	.get_sei_data   = NULL,
	.get_video_info = NULL
};
