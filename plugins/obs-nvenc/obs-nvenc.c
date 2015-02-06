/******************************************************************************
    Copyright (C) 2014 by Sean Nelson <audiohacked@gmail.com>

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

#ifndef _STDINT_H_INCLUDED
#define _STDINT_H_INCLUDED
#endif

/* ------------------------------------------------------------------------- */

static const char *obs_nvenc_getname(void)
{
	return "NVENC";
}

static void *obs_nvenc_create(obs_data_t *settings, obs_encoder_t *encoder)
{
	struct obs_nvenc *obsnv = bzalloc(sizeof(struct obs_nvenc));
	obsnv->encoder = encoder;

	//obs_nvenc_helper_create_instance()
	obs_nvenc_helper_open_session(obsnv);
	
	return obsnv;
}

static void obs_nvenc_destroy(void *data)
{
	struct obs_nvenc *obsnv = data;

	if (obsnv) {
		os_end_high_performance(obsnv->performance_token);
		//clear_data(obsnv);
		da_free(obsnv->packet_data);
		bfree(obsnv);
	}
}

static bool obs_nvenc_encode(void *data, struct encoder_frame *frame,
		struct encoder_packet *packet, bool *received_packet)
{
	struct obs_nvenc *obsnv = data;

	return false;
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
