#include "ads1299_reference_app.h"
#include "generic_board_port.h"

int main(void) {
    const ads1299_reference_board_t *board = ads1299_board_instance();
    ads1299_reference_app_t app;
    ads1299_status_t rc = ads1299_reference_app_init(&app, board);
    if (rc != ADS1299_OK) {
        if (board && board->fatal) board->fatal(board->user, "ADS1299 init failed");
        for (;;) { }
    }

    for (;;) {
        rc = ads1299_reference_app_step(&app);
        if (rc != ADS1299_OK && rc != ADS1299_ETIMEOUT) {
            if (board->fatal) board->fatal(board->user, "ADS1299 acquisition failed");
            for (;;) { }
        }
    }
}
