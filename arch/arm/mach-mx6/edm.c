
#include <asm-generic/errno-base.h>
#include <linux/module.h>

#include "edm.h"

int edm_external_gpio[((EDM_N_EXTERNAL_GPIO) < 8 ? 8 : (EDM_N_EXTERNAL_GPIO))] = {
        [0] = -EINVAL,
        [1] = -EINVAL,
        [2] = -EINVAL,
        [3] = -EINVAL,
        [4] = -EINVAL,
        [5] = -EINVAL,
        [6] = -EINVAL,
        [7] = -EINVAL,
};
EXPORT_SYMBOL_GPL(edm_external_gpio);

int edm_i2c[3] = { -EINVAL, -EINVAL, -EINVAL };
EXPORT_SYMBOL_GPL(edm_i2c);

int edm_ddc = -EINVAL;
EXPORT_SYMBOL_GPL(edm_ddc);

/* Optional; for audio codecs that needs it */
void *edm_analog_audio_platform_data = 0;
EXPORT_SYMBOL_GPL(edm_analog_audio_platform_data);

