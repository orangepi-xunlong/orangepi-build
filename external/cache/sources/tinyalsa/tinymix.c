/* tinymix.c
**
** Copyright 2011, The Android Open Source Project
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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

static void tinymix_list_controls(struct mixer *mixer);
static void tinymix_detail_control(struct mixer *mixer, const char *control,
                                   int print_all);
static void tinymix_set_value(struct mixer *mixer, const char *control,
                              char **values, unsigned int num_values);
static void tinymix_print_enum(struct mixer_ctl *ctl, int print_all);

int main(int argc, char **argv)
{
    struct mixer *mixer;
    int card = 0;

    if ((argc > 2) && (strcmp(argv[1], "-D") == 0)) {
        argv++;
        if (argv[1]) {
            card = atoi(argv[1]);
            argv++;
            argc -= 2;
        } else {
            argc -= 1;
        }
    }

    mixer = mixer_open(card);
    if (!mixer) {
        fprintf(stderr, "Failed to open mixer\n");
        return EXIT_FAILURE;
    }


    if (argc == 1) {
        printf("Mixer name: '%s'\n", mixer_get_name(mixer));
        tinymix_list_controls(mixer);
    } else if (argc == 2) {
        tinymix_detail_control(mixer, argv[1], 1);
    } else if (argc >= 3) {
        tinymix_set_value(mixer, argv[1], &argv[2], argc - 2);
    } else {
        printf("Usage: tinymix [-D card] [control id] [value to set]\n");
    }

    mixer_close(mixer);

    return 0;
}

static void tinymix_list_controls(struct mixer *mixer)
{
    struct mixer_ctl *ctl;
    const char *name, *type;
    unsigned int num_ctls, num_values;
    unsigned int i;

    num_ctls = mixer_get_num_ctls(mixer);

    printf("Number of controls: %d\n", num_ctls);

    printf("ctl\ttype\tnum\t%-40s value\n", "name");
    for (i = 0; i < num_ctls; i++) {
        ctl = mixer_get_ctl(mixer, i);

        name = mixer_ctl_get_name(ctl);
        type = mixer_ctl_get_type_string(ctl);
        num_values = mixer_ctl_get_num_values(ctl);
        printf("%d\t%s\t%d\t%-40s", i, type, num_values, name);
        tinymix_detail_control(mixer, name, 0);
    }
}

static void tinymix_print_enum(struct mixer_ctl *ctl, int print_all)
{
    unsigned int num_enums;
    unsigned int i;
    const char *string;

    num_enums = mixer_ctl_get_num_enums(ctl);

    for (i = 0; i < num_enums; i++) {
        string = mixer_ctl_get_enum_string(ctl, i);
        if (print_all)
            printf("\t%s%s", mixer_ctl_get_value(ctl, 0) == (int)i ? ">" : "",
                   string);
        else if (mixer_ctl_get_value(ctl, 0) == (int)i)
            printf(" %-s", string);
    }
}

static void tinymix_detail_control(struct mixer *mixer, const char *control,
                                   int print_all)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_values;
    unsigned int i;
    int min, max;
    int ret;
    char buf[512] = { 0 };
    size_t len;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control\n");
        return;
    }

    type = mixer_ctl_get_type(ctl);
    num_values = mixer_ctl_get_num_values(ctl);

    if (type == MIXER_CTL_TYPE_BYTE) {
        len = num_values;
        if (len > sizeof(buf)) {
            fprintf(stderr, "Truncating get to %zu bytes\n", sizeof(buf));
            len = sizeof(buf);
        }
        ret = mixer_ctl_get_array(ctl, buf, len);
        if (ret < 0) {
            fprintf(stderr, "Failed to mixer_ctl_get_array\n");
            return;
        }
    }

    if (print_all)
        printf("%s:", mixer_ctl_get_name(ctl));

    for (i = 0; i < num_values; i++) {
        switch (type)
        {
        case MIXER_CTL_TYPE_INT:
            printf(" %d", mixer_ctl_get_value(ctl, i));
            break;
        case MIXER_CTL_TYPE_BOOL:
            printf(" %s", mixer_ctl_get_value(ctl, i) ? "On" : "Off");
            break;
        case MIXER_CTL_TYPE_ENUM:
            tinymix_print_enum(ctl, print_all);
            break;
        case MIXER_CTL_TYPE_BYTE:
            printf("%02x", buf[i]);
            break;
        default:
            printf(" unknown");
            break;
        };
    }

    if (print_all) {
        if (type == MIXER_CTL_TYPE_INT) {
            min = mixer_ctl_get_range_min(ctl);
            max = mixer_ctl_get_range_max(ctl);
            printf(" (range %d->%d)", min, max);
        }
    }
    printf("\n");
}

static void tinymix_set_byte_ctl(struct mixer_ctl *ctl, const char *control,
    char **values, unsigned int num_values)
{
    int ret;
    char buf[512] = { 0 };
    char *end;
    int i;
    long n;

    if (num_values > sizeof(buf)) {
        fprintf(stderr, "Truncating set to %zu bytes\n", sizeof(buf));
        num_values = sizeof(buf);
    }

    for (i = 0; i < num_values; i++) {
        errno = 0;
        n = strtol(values[i], &end, 0);
        if (*end) {
            fprintf(stderr, "%s not an integer\n", values[i]);
            exit(EXIT_FAILURE);
        }
        if (errno) {
            fprintf(stderr, "strtol: %s: %s\n", values[i],
                strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (n < 0 || n > 0xff) {
            fprintf(stderr, "%s should be between [0, 0xff]\n",
                values[i]);
            exit(EXIT_FAILURE);
        }
        buf[i] = n;
    }

    ret = mixer_ctl_set_array(ctl, buf, num_values);
    if (ret < 0) {
        fprintf(stderr, "Failed to set binary control\n");
        exit(EXIT_FAILURE);
    }
}

static int is_int(char *value)
{
    char* end;
    long int result;

    errno = 0;
    result = strtol(value, &end, 10);

    if (result == LONG_MIN || result == LONG_MAX)
        return 0;

    return errno == 0 && *end == '\0';
}

static void tinymix_set_value(struct mixer *mixer, const char *control,
                              char **values, unsigned int num_values)
{
    struct mixer_ctl *ctl;
    enum mixer_ctl_type type;
    unsigned int num_ctl_values;
    unsigned int i;

    if (isdigit(control[0]))
        ctl = mixer_get_ctl(mixer, atoi(control));
    else
        ctl = mixer_get_ctl_by_name(mixer, control);

    if (!ctl) {
        fprintf(stderr, "Invalid mixer control\n");
        return;
    }

    type = mixer_ctl_get_type(ctl);
    num_ctl_values = mixer_ctl_get_num_values(ctl);

    if (type == MIXER_CTL_TYPE_BYTE) {
        tinymix_set_byte_ctl(ctl, control, values, num_values);
        return;
    }

    if (is_int(values[0])) {
        if (num_values == 1) {
            /* Set all values the same */
            int value = atoi(values[0]);

            for (i = 0; i < num_ctl_values; i++) {
                if (mixer_ctl_set_value(ctl, i, value)) {
                    fprintf(stderr, "Error: invalid value\n");
                    return;
                }
            }
        } else {
            /* Set multiple values */
            if (num_values > num_ctl_values) {
                fprintf(stderr,
                        "Error: %d values given, but control only takes %d\n",
                        num_values, num_ctl_values);
                return;
            }
            for (i = 0; i < num_values; i++) {
                if (mixer_ctl_set_value(ctl, i, atoi(values[i]))) {
                    fprintf(stderr, "Error: invalid value for index %d\n", i);
                    return;
                }
            }
        }
    } else {
        if (type == MIXER_CTL_TYPE_ENUM) {
            if (num_values != 1) {
                fprintf(stderr, "Enclose strings in quotes and try again\n");
                return;
            }
            if (mixer_ctl_set_enum_by_string(ctl, values[0]))
                fprintf(stderr, "Error: invalid enum value\n");
        } else {
            fprintf(stderr, "Error: only enum types can be set with strings\n");
        }
    }
}

