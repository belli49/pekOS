#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int printf(const char* restrict format, ...) {
	va_list parameters;
	va_start(parameters, format);

	int written = 0;

	while (*format != '\0') {
		size_t maxrem = INT_MAX - written;

		if (format[0] != '%' || format[1] == '%') {
			if (format[0] == '%')
				format++;
			size_t amount = 1;
			while (format[amount] && format[amount] != '%')
				amount++;
			if (maxrem < amount) {
				// TODO: Set errno to EOVERFLOW.
				return -1;
			}
			if (!print(format, amount))
				return -1;
			format += amount;
			written += amount;
			continue;
		}

		const char* format_begun_at = format++;
    size_t len;

    // TODO: format into functions
    switch (*format) {
      case 'c': {
        format++;
        char c = (char) va_arg(parameters, int /* char promotes to int */);

        if (!maxrem) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }

        if (!print(&c, sizeof(c)))
          return -1;

        written++;
      } break;

      case 's': {
        format++;
        const char* str = va_arg(parameters, const char*);
        len = strlen(str);

        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }
        
        if (!print(str, len))
          return -1;

        written += len;
      } break;

      case 'd': {
        format++;
        int num = (int) va_arg(parameters, int);
        char num_to_print[32];
        len = 0;
        
        // INT_MIN cannot be turned into positive
        if (num == INT_MIN) {
            printf("-2147483648");
            written += 11;
            break;
        }

        if (!num) {
          char temp = '0';
          if (!print(&temp, sizeof(temp)))
            return -1;
          written++;
          break;
        }

        bool neg = false;
        if (num < 0) {
          neg = true;
          num = -num;
        }

        while (num) {
          num_to_print[len] = (num % 10) + '0';
          num /= 10;
          len++;
        }

        if (maxrem < len + neg) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }

        if (neg) {
          char minus = '-';
          if (!print(&minus, sizeof(minus)))
            return -1;
        }

        for (int printed = len - 1; printed >= 0; printed--) {
          if (!print(num_to_print + printed, sizeof(char)))
            return -1;
        }

        written += len + neg; // ("-") + num_to_print
      } break;

      case 'u': {
        format++;
        uint32_t unum = (uint32_t) va_arg(parameters, uint32_t);
        char unum_to_print[32];
        len = 0;

        if (!unum) {
          char temp = '0';
          if (!print(&temp, sizeof(temp)))
            return -1;
          written++;
          break;
        }

        while (unum) {
          unum_to_print[len] = (unum % 10) + '0';
          unum /= 10;
          len++;
        }

        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }

        for (int printed = len - 1; printed >= 0; printed--) {
          if (!print(unum_to_print + printed, sizeof(char)))
            return -1;
        }

        written += len;
      } break;

      case 'x': {
        format++;
        uint32_t hex = (uint32_t) va_arg(parameters, uint32_t);
        char hex_to_print[8];
        len = 0;
        
        if (!hex) {
          printf("0x0");
          written += 3;
          break;
        }

        while (hex) {
          int temp = hex % 16;
          hex_to_print[len] = (temp) + (temp < 10 ? '0' : ('a' - 10));
          hex /= 16;
          len++;
        }

        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }

        printf("0x");

        for (int printed = len - 1; printed >= 0; printed--) {
          if (!print(hex_to_print + printed, sizeof(char)))
            return -1;
        }

        written += len + 2; // "0x" + hex_to_print
      } break;

      default: {
        format = format_begun_at;
        len = strlen(format);

        if (maxrem < len) {
          // TODO: Set errno to EOVERFLOW.
          return -1;
        }

        if (!print(format, len))
          return -1;

        written += len;
        format += len;
       } break;
    }
	}

	va_end(parameters);
	return written;
}
