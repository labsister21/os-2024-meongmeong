#ifndef CMOS_DRIVER_H
#define CMOS_DRIVER_H

#include <stdint.h>

// CMOS I/O Ports
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

// CMOS Registers
#define CMOS_REG_SECONDS 0x00
#define CMOS_REG_MINUTES 0x02
#define CMOS_REG_HOURS 0x04
#define CMOS_REG_WEEKDAY 0x06
#define CMOS_REG_DAY 0x07
#define CMOS_REG_MONTH 0x08
#define CMOS_REG_YEAR 0x09

// Inline functions for port I/O
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71

// CMOS Registers
#define CMOS_REG_SECONDS 0x00
#define CMOS_REG_MINUTES 0x02
#define CMOS_REG_HOURS 0x04
#define CMOS_REG_WEEKDAY 0x06
#define CMOS_REG_DAY 0x07
#define CMOS_REG_MONTH 0x08
#define CMOS_REG_YEAR 0x09

// Inline functions for port I/O
static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Disable interrupts (for critical section)
static inline void disable_interrupts()
{
    __asm__ volatile("cli");
}

// Enable interrupts
static inline void enable_interrupts()
{
    __asm__ volatile("sti");
}

// Read from CMOS
static inline uint8_t cmos_read(uint8_t reg)
{

    outb(CMOS_ADDRESS, reg);
    uint8_t data = inb(CMOS_DATA);

    return data;
}

// Write to CMOS
static inline void cmos_write(uint8_t reg, uint8_t data)
{
    disable_interrupts();
    outb(CMOS_ADDRESS, reg);
    outb(CMOS_DATA, data);
    enable_interrupts();
}

// Convert BCD to binary
static inline uint8_t bcd_to_bin(uint8_t bcd)
{
    return ((bcd / 16) * 10) + (bcd % 16);
}

// Example: Read current time from CMOS
static inline void read_cmos_time(uint8_t *seconds, uint8_t *minutes, uint8_t *hours)
{
    *seconds = bcd_to_bin(cmos_read(CMOS_REG_SECONDS));
    *minutes = bcd_to_bin(cmos_read(CMOS_REG_MINUTES));
    *hours = bcd_to_bin(cmos_read(CMOS_REG_HOURS));
}

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
#endif