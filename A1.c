#include <stdio.h>
#include <string.h>
#include <ctype.h>

int extractIPv4(const char* str, unsigned long* outAddress, int* outPort)
{
    int i = 0;

    *outAddress = 0;
    *outPort = -1;

    while (str[i] != '\0') {
        int start;
        int octets[4];
        int octetIndex = 0;
        int valid = 1;
        int j;

        /*
         * A candidate can only begin with a digit.
         * Everything else is garbage and is skipped.
         */
        if (!isdigit((unsigned char)str[i])) {
            i++;
            continue;
        }

        start = i;

        /* Parse exactly four octets. */
        while (octetIndex < 4) {
            int value = 0;
            int digits = 0;

            /* Each octet must contain 1-3 digits. */
            while (isdigit((unsigned char)str[i])) {
                if (digits >= 3) {
                    valid = 0;
                    break;
                }

                value = value * 10 + (str[i] - '0');
                digits++;
                i++;
            }

            if (!valid || digits == 0) {
                valid = 0;
                break;
            }

            /* No leading zero unless the octet is exactly 0. */
            if (digits > 1 && str[i - digits] == '0') {
                valid = 0;
                break;
            }

            /* Octet must be between 0 and 255. */
            if (value > 255) {
                valid = 0;
                break;
            }

            octets[octetIndex] = value;
            octetIndex++;

            if (octetIndex < 4) {
                if (str[i] != '.') {
                    valid = 0;
                    break;
                }

                i++;
            }
        }

        if (!valid) {
            i = start + 1;
            continue;
        }

        /* Check for an optional port. */
        int port = -1;

        if (str[i] == ':') {
            int portValue = 0;
            int portDigits = 0;

            i++;

            while (isdigit((unsigned char)str[i])) {
                if (portDigits >= 5) {
                    valid = 0;
                    break;
                }

                portValue = portValue * 10 + (str[i] - '0');
                portDigits++;
                i++;
            }

            if (!valid || portDigits == 0) {
                valid = 0;
            }

            /* No leading zero unless port is exactly 0. */
            if (valid && portDigits > 1 &&
                str[i - portDigits] == '0') {
                valid = 0;
            }

            /* Port must be 0-65535. */
            if (valid && portValue > 65535) {
                valid = 0;
            }

            if (valid) {
                port = portValue;
            }
        }

        /*
         * A valid token cannot have another digit, period, or colon
         * immediately after it.
         */
        if (valid && (isdigit((unsigned char)str[i]) ||
                      str[i] == '.' ||
                      str[i] == ':')) {
            valid = 0;
        }

        if (!valid) {
            i = start + 1;
            continue;
        }

        /* Build the 32-bit decimal IPv4 value. */
        *outAddress =
            ((unsigned long)octets[0] << 24) |
            ((unsigned long)octets[1] << 16) |
            ((unsigned long)octets[2] << 8) |
            (unsigned long)octets[3];

        *outPort = port;

        return 1;
    }

    return 0;
}

int main(void)
{
    char input[1000];
    unsigned long address;
    int port;

    while (1) {
        printf("Enter a string (or 'END' to quit): ");

        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "END") == 0) {
            printf("Program terminated.\n");
            break;
        }

        if (extractIPv4(input, &address, &port)) {
            unsigned int a;
            unsigned int b;
            unsigned int c;
            unsigned int d;

            a = (address >> 24) & 255;
            b = (address >> 16) & 255;
            c = (address >> 8) & 255;
            d = address & 255;

            printf(
                "Extracted IPv4 address: %u.%u.%u.%u "
                "(decimal value: %lu, port: ",
                a, b, c, d, address
            );

            if (port == -1) {
                printf("none)\n");
            } else {
                printf("%d)\n", port);
            }
        } else {
            printf("Invalid input: no valid IPv4 address found\n");
        }
    }

    return 0;
}