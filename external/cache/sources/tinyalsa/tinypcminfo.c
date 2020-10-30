/* tinypcminfo.c
**
** Copyright 2012, The Android Open Source Project
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**     * Neither the name of The Android Open Source Project nor the names of
**       its contributors may be used to endorse or promote products derived
**       from this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY The Android Open Source Project ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL The Android Open Source Project BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
** CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
*/

#include <tinyalsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#endif

/* The format_lookup is in order of SNDRV_PCM_FORMAT_##index and
 * matches the grouping in sound/asound.h.  Note this is not
 * continuous and has an empty gap from (25 - 30).
 */
static const char *format_lookup[] = {
        /*[0] =*/ "S8",
        "U8",
        "S16_LE",
        "S16_BE",
        "U16_LE",
        "U16_BE",
        "S24_LE",
        "S24_BE",
        "U24_LE",
        "U24_BE",
        "S32_LE",
        "S32_BE",
        "U32_LE",
        "U32_BE",
        "FLOAT_LE",
        "FLOAT_BE",
        "FLOAT64_LE",
        "FLOAT64_BE",
        "IEC958_SUBFRAME_LE",
        "IEC958_SUBFRAME_BE",
        "MU_LAW",
        "A_LAW",
        "IMA_ADPCM",
        "MPEG",
        /*[24] =*/ "GSM",
        [31] = "SPECIAL",
        "S24_3LE",
        "S24_3BE",
        "U24_3LE",
        "U24_3BE",
        "S20_3LE",
        "S20_3BE",
        "U20_3LE",
        "U20_3BE",
        "S18_3LE",
        "S18_3BE",
        "U18_3LE",
        /*[43] =*/ "U18_3BE",
#if 0
        /* recent additions, may not be present on local asound.h */
        "G723_24",
        "G723_24_1B",
        "G723_40",
        "G723_40_1B",
        "DSD_U8",
        "DSD_U16_LE",
#endif
};

/* Returns a human readable name for the format associated with bit_index,
 * NULL if bit_index is not known.
 */
inline const char *pcm_get_format_name(unsigned bit_index)
{
    return bit_index < ARRAY_SIZE(format_lookup) ? format_lookup[bit_index] : NULL;
}

int main(int argc, char **argv)
{
    unsigned int device = 0;
    unsigned int card = 0;
    int i;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s -D card -d device\n", argv[0]);
        return 1;
    }

    /* parse command line arguments */
    argv += 1;
    while (*argv) {
        if (strcmp(*argv, "-D") == 0) {
            argv++;
            if (*argv)
                card = atoi(*argv);
        }
        if (strcmp(*argv, "-d") == 0) {
            argv++;
            if (*argv)
                device = atoi(*argv);
        }
        if (*argv)
            argv++;
    }

    printf("Info for card %d, device %d:\n", card, device);

    for (i = 0; i < 2; i++) {
        struct pcm_params *params;
        struct pcm_mask *m;
        unsigned int min;
        unsigned int max;

        printf("\nPCM %s:\n", i == 0 ? "out" : "in");

        params = pcm_params_get(card, device, i == 0 ? PCM_OUT : PCM_IN);
        if (params == NULL) {
            printf("Device does not exist.\n");
            continue;
        }

        m = pcm_params_get_mask(params, PCM_PARAM_ACCESS);
        if (m) { /* bitmask, refer to SNDRV_PCM_ACCESS_*, generally interleaved */
            printf("      Access:\t%#08x\n", m->bits[0]);
        }
        m = pcm_params_get_mask(params, PCM_PARAM_FORMAT);
        if (m) { /* bitmask, refer to: SNDRV_PCM_FORMAT_* */
            unsigned j, k, count = 0;
            const unsigned bitcount = sizeof(m->bits[0]) * 8;

            /* we only check first two format masks (out of 8) - others are zero. */
            printf("   Format[0]:\t%#08x\n", m->bits[0]);
            printf("   Format[1]:\t%#08x\n", m->bits[1]);

            /* print friendly format names, if they exist */
            for (k = 0; k < 2; ++k) {
                for (j = 0; j < bitcount; ++j) {
                    const char *name;

                    if (m->bits[k] & (1 << j)) {
                        name = pcm_get_format_name(j + k*bitcount);
                        if (name) {
                            if (count++ == 0) {
                                printf(" Format Name:\t");
                            } else {
                                printf (", ");
                            }
                            printf("%s", name);
                        }
                    }
                }
            }
            if (count) {
                printf("\n");
            }
        }
        m = pcm_params_get_mask(params, PCM_PARAM_SUBFORMAT);
        if (m) { /* bitmask, should be 1: SNDRV_PCM_SUBFORMAT_STD */
            printf("   Subformat:\t%#08x\n", m->bits[0]);
        }
        min = pcm_params_get_min(params, PCM_PARAM_RATE);
        max = pcm_params_get_max(params, PCM_PARAM_RATE);
        printf("        Rate:\tmin=%uHz\tmax=%uHz\n", min, max);
        min = pcm_params_get_min(params, PCM_PARAM_CHANNELS);
        max = pcm_params_get_max(params, PCM_PARAM_CHANNELS);
        printf("    Channels:\tmin=%u\t\tmax=%u\n", min, max);
        min = pcm_params_get_min(params, PCM_PARAM_SAMPLE_BITS);
        max = pcm_params_get_max(params, PCM_PARAM_SAMPLE_BITS);
        printf(" Sample bits:\tmin=%u\t\tmax=%u\n", min, max);
        min = pcm_params_get_min(params, PCM_PARAM_PERIOD_SIZE);
        max = pcm_params_get_max(params, PCM_PARAM_PERIOD_SIZE);
        printf(" Period size:\tmin=%u\t\tmax=%u\n", min, max);
        min = pcm_params_get_min(params, PCM_PARAM_PERIODS);
        max = pcm_params_get_max(params, PCM_PARAM_PERIODS);
        printf("Period count:\tmin=%u\t\tmax=%u\n", min, max);

        pcm_params_free(params);
    }

    return 0;
}
