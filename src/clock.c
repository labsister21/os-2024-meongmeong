
#include "header/cmos.h"
#include "header/shell/utils/shellutils.h"

void print_cmos_time()
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    // Read time from CMOS
    read_cmos_time(&seconds, &minutes, &hours);

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
