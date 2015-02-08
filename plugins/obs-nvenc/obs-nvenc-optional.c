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

void obs_nvenc_get_defaults(obs_data_t *settings)
{
	obs_data_set_default_int   (settings, "bitrate",     3500);
	obs_data_set_default_bool  (settings, "use_bufsize", false);
	obs_data_set_default_int   (settings, "buffer_size", 3500);
	obs_data_set_default_int   (settings, "keyint_sec",  0);
	obs_data_set_default_int   (settings, "crf",         23);
	obs_data_set_default_bool  (settings, "cbr",         true);

	obs_data_set_default_string(settings, "preset",      "veryfast");
	obs_data_set_default_string(settings, "profile",     "");
	obs_data_set_default_string(settings, "tune",        "");
}

obs_properties_t *obs_nvenc_get_properties(void *unused)
{
	UNUSED_PARAMETER(unused);

	obs_properties_t *props = obs_properties_create();
//	obs_property_t *list;
//	obs_property_t *p;

	//obs_properties_add_int(props, "bitrate", obs_module_text("Bitrate"));
	return props;
}

bool obs_nvenc_update(void *data, obs_data_t *settings)
{
	struct obs_nvenc *obsnv = data;
	bool success = false;
	NVENCSTATUS nvStatus = NV_ENC_ERR_GENERIC;

	//success = update_settings(obsnv, settings);
	if (success) {
		//nvStatus = nvenc_encoder_reconfig(obsnv->nvenc_device, &obsnv->nvenc_picture);
		if (nvStatus != NV_ENC_SUCCESS) {
			warn("Failed to reconfigure: %d", nvStatus);
		}
		return nvStatus == NV_ENC_SUCCESS;
	}
	return false;
}

bool obs_nvenc_get_extra_data(void *data, uint8_t **extra_data, size_t *size)
{
	struct obs_nvenc *obsnv = data;

	if (!obsnv->nvenc_device) {
		return false;
	}

	*extra_data = obsnv->extra_data;
	*size       = obsnv->extra_data_size;
	return true;
}

bool obs_nvenc_get_sei_data(void *data, uint8_t **sei, size_t *size)
{
	struct obs_nvenc *obsnv = data;
	
	if (!obsnv->nvenc_device) {
		return false;
	}

	*sei = obsnv->sei;
	*size = obsnv->sei_size;
	return true;
}

bool obs_nvenc_get_video_info(void *data, struct video_scale_info *info)
{
	struct obs_nvenc *obsnv = data;
	video_t *video = obs_encoder_video(obsnv->encoder);
	const struct video_output_info *vid_info = video_output_get_info(video);

	if (vid_info->format == VIDEO_FORMAT_I420 ||
		vid_info->format == VIDEO_FORMAT_NV12)
		return false;

	info->format = VIDEO_FORMAT_NV12;
	info->width = vid_info->width;
	info->height = vid_info->height;
	info->range = vid_info->range;
	info->colorspace = vid_info->colorspace;

	return true;
}