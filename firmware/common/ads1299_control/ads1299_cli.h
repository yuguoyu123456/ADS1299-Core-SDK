#ifndef ADS1299_CLI_H
#define ADS1299_CLI_H

#include <stddef.h>

#include "ads1299_control.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_CLI_LINE_MAX 192u

typedef void (*ads1299_cli_write_fn)(void *user, const char *text);

typedef struct {
    ads1299_control_t *control;
    ads1299_cli_write_fn write;
    void *user;
} ads1299_cli_t;

ads1299_status_t ads1299_cli_init(ads1299_cli_t *cli,
                                  ads1299_control_t *control,
                                  ads1299_cli_write_fn write,
                                  void *user);

/** Execute one NUL-terminated command line. */
ads1299_status_t ads1299_cli_execute(ads1299_cli_t *cli, const char *line);

/** Print the command summary through the configured writer callback. */
void ads1299_cli_print_help(ads1299_cli_t *cli);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_CLI_H */
