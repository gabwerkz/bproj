/*
 * smdk_wm9713.c  --  SoC audio for SMDK
 *
 * Copyright 2010 Samsung Electronics Co. Ltd.
 * Author: Jaswinder Singh Brar <jassi.brar@samsung.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/soc.h>

#include "../codecs/wm9713.h"
#include "s3c-dma.h"
#include "s3c-ac97.h"

static struct snd_soc_card smdk;

/*
 * Default CFG switch settings to use this driver:
 *
 *   SMDK6410: Set CFG1 1-3 On, CFG2 1-4 Off
 *   SMDKC100: Set CFG6 1-3 On, CFG7 1   On
 *   SMDKC110: Set CFGB10 1-2 Off, CFGB12 1-3 On
 *   SMDKV210: Set CFGB10 1-2 Off, CFGB12 1-3 On
 */

/*
 Playback (HeadPhone):-
	$ amixer sset 'Headphone' unmute
	$ amixer sset 'Right Headphone Out Mux' 'Headphone'
	$ amixer sset 'Left Headphone Out Mux' 'Headphone'
	$ amixer sset 'Right HP Mixer PCM' unmute
	$ amixer sset 'Left HP Mixer PCM' unmute

 Capture (LineIn):-
	$ amixer sset 'Right Capture Source' 'Line'
	$ amixer sset 'Left Capture Source' 'Line'
*/

static struct snd_soc_dai_link smdk_dai = {
	.name = "AC97",
	.stream_name = "AC97 PCM",
	.platform_name = "s3c24xx-pcm-audio",
	.cpu_dai_name = "s3c-ac97-dai",
	.codec_dai_name = "wm9713-hifi",
	.codec_name = "wm9713-codec",
};

static struct snd_soc_card smdk = {
	.name = "SMDK",
	.dai_link = &smdk_dai,
	.num_links = 1,
};

static struct platform_device *smdk_snd_wm9713_device;
static struct platform_device *smdk_snd_ac97_device;

static int __init smdk_init(void)
{
	int ret;

	smdk_snd_wm9713_device = platform_device_alloc("wm9713-codec", -1);
	if (!smdk_snd_wm9713_device)
		return -ENOMEM;

	ret = platform_device_add(smdk_snd_wm9713_device);
	if (ret)
		goto err;

	smdk_snd_ac97_device = platform_device_alloc("soc-audio", -1);
	if (!smdk_snd_ac97_device) {
		ret = -ENOMEM;
		goto err;
	}

	platform_set_drvdata(smdk_snd_ac97_device, &smdk);

	ret = platform_device_add(smdk_snd_ac97_device);
	if (ret) {
		platform_device_put(smdk_snd_ac97_device);
		goto err;
	}

	return 0;
err:
	platform_device_put(smdk_snd_wm9713_device);
	return ret;
}

static void __exit smdk_exit(void)
{
	platform_device_unregister(smdk_snd_ac97_device);
}

module_init(smdk_init);
module_exit(smdk_exit);

/* Module information */
MODULE_AUTHOR("Jaswinder Singh Brar, jassi.brar@samsung.com");
MODULE_DESCRIPTION("ALSA SoC SMDK+WM9713");
MODULE_LICENSE("GPL");
