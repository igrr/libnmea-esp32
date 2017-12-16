/* NMEA parsing example for ESP32.
 * Based on "parse_stdin.c" example from libnmea.
 * Copyright (c) 2015 Jack Engqvist Johansson.
 * Additions Copyright (c) 2017 Ivan Grokhotkov.
 * See "LICENSE" file in libnmea directory for license.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "nmea.h"
#include "gpgll.h"
#include "gpgga.h"
#include "gprmc.h"

#define UART_NUM                UART_NUM_2
#define UART_RX_PIN             21
#define UART_RX_BUF_SIZE        (1024)

static void uart_setup();
static void read_and_parse_nmea();

void app_main()
{
    uart_setup();
    read_and_parse_nmea();
}

static void uart_setup()
{
    uart_config_t uart_config = {
            .baud_rate = 9600,
            .data_bits = UART_DATA_8_BITS,
            .parity = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM,
            UART_PIN_NO_CHANGE, 21,
            UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, UART_RX_BUF_SIZE * 2, 0, 0, NULL, 0);
}


static void read_and_parse_nmea()
{
    // Configure a temporary buffer for the incoming data
    char *buffer = (char*) malloc(UART_RX_BUF_SIZE + 1);
    char fmt_buf[32];
    size_t total_bytes = 0;
    while (1) {
        // Read data from the UART
        int read_bytes = uart_read_bytes(UART_NUM,
                (uint8_t*) buffer + total_bytes,
                UART_RX_BUF_SIZE - total_bytes, 100 / portTICK_RATE_MS);
        if (read_bytes <= 0) {
            continue;
        }

        nmea_s *data;
        total_bytes += read_bytes;

        /* find start (a dollar sign) */
        char* start = memchr(buffer, '$', total_bytes);
        if (start == NULL) {
            total_bytes = 0;
            continue;
        }

        /* find end of line */
        char* end = memchr(start, '\r', total_bytes - (start - buffer));
        if (NULL == end || '\n' != *(++end)) {
            continue;
        }
        end[-1] = NMEA_END_CHAR_1;
        end[0] = NMEA_END_CHAR_2;

        /* handle data */
        data = nmea_parse(start, end - start + 1, 0);
        if (data != NULL) {
            if (data->errors != 0) {
                printf("WARN: The sentence struct contains parse errors!\n");
            }

            if (NMEA_GPGGA == data->type) {
                printf("GPGGA sentence\n");
                nmea_gpgga_s *gpgga = (nmea_gpgga_s *) data;
                printf("Number of satellites: %d\n", gpgga->n_satellites);
                printf("Altitude: %d %c\n", gpgga->altitude,
                        gpgga->altitude_unit);
            }

            if (NMEA_GPGLL == data->type) {
                printf("GPGLL sentence\n");
                nmea_gpgll_s *pos = (nmea_gpgll_s *) data;
                printf("Longitude:\n");
                printf("  Degrees: %d\n", pos->longitude.degrees);
                printf("  Minutes: %f\n", pos->longitude.minutes);
                printf("  Cardinal: %c\n", (char) pos->longitude.cardinal);
                printf("Latitude:\n");
                printf("  Degrees: %d\n", pos->latitude.degrees);
                printf("  Minutes: %f\n", pos->latitude.minutes);
                printf("  Cardinal: %c\n", (char) pos->latitude.cardinal);
                strftime(fmt_buf, sizeof(fmt_buf), "%H:%M:%S", &pos->time);
                printf("Time: %s\n", fmt_buf);
            }

            if (NMEA_GPRMC == data->type) {
                printf("GPRMC sentence\n");
                nmea_gprmc_s *pos = (nmea_gprmc_s *) data;
                printf("Longitude:\n");
                printf("  Degrees: %d\n", pos->longitude.degrees);
                printf("  Minutes: %f\n", pos->longitude.minutes);
                printf("  Cardinal: %c\n", (char) pos->longitude.cardinal);
                printf("Latitude:\n");
                printf("  Degrees: %d\n", pos->latitude.degrees);
                printf("  Minutes: %f\n", pos->latitude.minutes);
                printf("  Cardinal: %c\n", (char) pos->latitude.cardinal);
                strftime(fmt_buf, sizeof(fmt_buf), "%H:%M:%S", &pos->time);
                printf("Time: %s\n", fmt_buf);
            }

            nmea_free(data);
        }

        /* buffer empty? */
        if (end == buffer + total_bytes) {
            total_bytes = 0;
            continue;
        }

        /* copy rest of buffer to beginning */
        if (buffer != memmove(buffer, end, total_bytes - (end - buffer))) {
            total_bytes = 0;
            continue;
        }

        total_bytes -= end - buffer;
    }
    free(buffer);
}
