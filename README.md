# NMEA parser component for ESP32 ESP-IDF

This is a wrapper around [libnmea](https://github.com/jacketizer/libnmea),
in the form of an [ESP-IDF](https://github.com/espressif/esp-idf) component.

To use, clone the component into the `components` directory of your project,
or add it as a submodule.
See [libnmea documentation](https://github.com/jacketizer/libnmea#how-to-use-it)
for more details.


## Example

Example project is provided inside `example` directory. It works the same way
as `parse_stdin.c` example from libnmea, except that it reads NMEA sentences
from UART. Connect TXD pin of GPS receiver to GPIO21 of ESP32, build and
flash the example. Decoded NMEA messages will be displayed in the console.

## Known issues

`strptime` function in newlib is less smart than the one in Glibc, and can not
parse format strings such as '%H%M%S' (i.e. when there are no separators
between numeric fields). Therefore date and time fields of NMEA messages
are not parsed.

## License

[libnmea](https://github.com/jacketizer/libnmea), this component, and the
example project are licensed under MIT License.
