#include "header/shell/utils/shellutils.h"

// Function to convert an integer to a string
static inline void int_to_str(int num, char *str)
{
    int i = 0;
    if (num == 0)
    {
        str[i++] = '0';
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    int is_negative = 0;
    if (num < 0)
    {
        is_negative = 1;
        num = -num;
    }

    int j = i;
    while (num > 0)
    {
        str[i++] = num % 10 + '0';
        num /= 10;
    }

    if (is_negative)
    {
        str[i++] = '-';
    }

    // If the number is a single digit and not negative, add a leading zero
    if (i - j == 1 && !is_negative)
    {
        str[i++] = '0';
    }

    str[i] = '\0';

    // Reverse the string
    int k = j;
    int l = i - 1;
    while (k < l)
    {
        char temp = str[k];
        str[k] = str[l];
        str[l] = temp;
        k++;
        l--;
    }
}

void print_cmos_time()
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    // Read time from CMOS
    syscall(14, (uint32_t) &seconds, (uint32_t) &minutes, (uint32_t) &hours);

    // Convert integers to strings
    char str_hours[3];
    char str_minutes[3];
    char str_seconds[3];

    char *ptr_hours = str_hours;
    char *ptr_minutes = str_minutes;
    char *ptr_seconds = str_seconds;

    int_to_str((hours + 7) % 24, ptr_hours); // Assuming the adjustment for timezone
    int_to_str(minutes, ptr_minutes);
    int_to_str(seconds, ptr_seconds);

    shell_put_clock(ptr_hours, ptr_minutes, ptr_seconds);
}

int main()
{
    // Example usage
    while (true)
    {
        print_cmos_time();
    }
    return 0;
}
