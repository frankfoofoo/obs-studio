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

#define SET_VER(configStruct, type) {configStruct.version = type##_VER;}

NVENCSTATUS obs_nvenc_helper_create_instance(void *data)
{
	struct obs_nvenc *obsnv = data;
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	void *nvEncodeAPICreateInstance;

	nvEncodeAPICreateInstance = os_dlsym(obs_nvenc_lib, "NvEncodeAPICreateInstance");
	if (nvEncodeAPICreateInstance == NULL)
		blog(LOG_ERROR, "Unable to create API Instance");

	memset(&obsnv->api, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
	return nvStatus;
}

NVENCSTATUS obs_nvenc_helper_open_session(void* data)
{
	struct obs_nvenc *obsnv = data;
	memset(&obsnv->session_params, 0, sizeof(obsnv->session_params));
	obsnv->session_params.version = NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS_VER;
	obsnv->session_params.device = 0;
	obsnv->session_params.deviceType = NV_ENC_DEVICE_TYPE_DIRECTX;
	obsnv->session_params.reserved = NULL;
	obsnv->session_params.apiVersion = NVENCAPI_VERSION;
	return obsnv->api->nvEncOpenEncodeSessionEx(&obsnv->session_params, &obsnv->nvenc_encoder);
}