/*
 * An exposure adjustment driver based on Qcom DSPP for OLED devices
 *
 * Copyright (C) 2012-2014, The Linux Foundation. All rights reserved.
 * Copyright (C) Sony Mobile Communications Inc. All rights reserved.
 * Copyright (C) 2014-2018, AngeloGioacchino Del Regno <kholk11@gmail.com>
 * Copyright (C) 2018, Devries <therkduan@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/notifier.h>
#include "dsi_display.h"
#include "dsi_panel.h"
#include "../sde/sde_crtc.h"
#include "../sde/sde_plane.h"
#include "exposure_adjustment.h"

static struct drm_msm_pcc pcc_blk = {0};
static bool pcc_backlight_enable = false;
static u32 last_level = ELVSS_OFF_THRESHOLD;

static int ea_panel_crtc_send_pcc(struct dsi_display *display,
			       u32 r_data, u32 g_data, u32 b_data)
{
	struct msm_drm_private *priv;
	struct drm_property *prop;
	struct drm_property_blob *pblob;
	struct drm_crtc *crtc = NULL;
	int rc;
	uint64_t val;

	if (!display->drm_conn) {
		pr_err("The display is not connected!!\n");
		return -EINVAL;
	};

	if (!display->drm_conn->state->crtc) {
		pr_err("No CRTC on display connector!!\n");
		return -ENODEV;
	}

	crtc = display->drm_conn->state->crtc;

	priv = crtc->dev->dev_private;
	prop = priv->cp_property[1]; /* SDE_CP_CRTC_DSPP_PCC == 1 !! */
	if (prop == NULL) {
		pr_err("FAIL! PCC is not supported!!?!?!\n");
		return -EINVAL;
	};
	pr_debug("prop->name = %s\n", prop->name);

	rc = sde_cp_crtc_get_property(crtc, prop, &val);
	if (rc) {
		pr_err("Cannot get CRTC property. Things may go wrong.\n");
	};

	pcc_blk.r.r = r_data;
	pcc_blk.g.g = g_data;
	pcc_blk.b.b = b_data;

	pblob = drm_property_create_blob(crtc->dev,
			sizeof(struct drm_msm_pcc), &pcc_blk);
	if (IS_ERR_OR_NULL(pblob)) {
		pr_err("Failed to create blob. Bailing out.\n");
		return -EINVAL;
	}
	pr_debug("DSPP Blob ID %d has length %zu\n",
			prop->base.id, pblob->length);

	rc = sde_cp_crtc_set_property(crtc, prop, pblob->base.id);
	if (rc) {
		pr_err("DSPP: Cannot set PCC: %d.\n", rc);
	}

	return rc;
}

static int ea_panel_send_pcc(u32 bl_lvl)
{
	struct dsi_display *display = NULL;
	u32 ea_coeff, r_data, g_data, b_data;

 	display = get_main_display();
	if (!display) {
		pr_err("ERROR: Cannot find display of this panel\n");
		return -ENODEV;
	}

	if (bl_lvl < ELVSS_OFF_THRESHOLD)
		ea_coeff = bl_lvl * PCC_BACKLIGHT_SCALE + EXPOSURE_ADJUSTMENT_MIN;
	else
		ea_coeff = EXPOSURE_ADJUSTMENT_MAX;

	pr_info("ea_coeff = %X\n", ea_coeff);
	r_data = ea_coeff;
	g_data = ea_coeff;
	b_data = ea_coeff;

	return ea_panel_crtc_send_pcc(display, r_data, g_data, b_data);
}

void ea_panel_mode_ctrl(struct dsi_panel *panel, bool enable)
{
	if (pcc_backlight_enable != enable) {
		pcc_backlight_enable = enable;
		pr_info("Recover backlight level = %d\n", last_level);
		dsi_panel_set_backlight(panel, last_level);
		if (!enable) {
			ea_panel_send_pcc(ELVSS_OFF_THRESHOLD);
		}
	} else if (last_level == 0 && !pcc_backlight_enable) {
		ea_panel_send_pcc(ELVSS_OFF_THRESHOLD);
	}
}

u32 ea_panel_calc_backlight(u32 bl_lvl)
{
	last_level = bl_lvl;

	if (pcc_backlight_enable && bl_lvl != 0 && bl_lvl < ELVSS_OFF_THRESHOLD) {
		if (ea_panel_send_pcc(bl_lvl))
			pr_err("ERROR: Failed to send PCC\n");

		return ELVSS_OFF_THRESHOLD;
	} else {
		return bl_lvl;
	}
}
