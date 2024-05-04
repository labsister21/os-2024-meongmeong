#include "header/cpu/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        /*TODO: Null Descriptor*/
        {
            // null
            .segment_low = 0,
            .base_low = 0,
            // Next 16-bit (Bit 32 to 07)
            .base_mid = 0,
            .type_bit = 0,
            .non_system = 0,
            // TODO= Continue SegmentDescriptor definition
            .dpl_bit = 0,
            .segment_present = 0,
            .segment_high = 0,
            .avail_bit = 0,
            .l_flag = 0,
            .db_flag = 0,
            .granularity = 0,
            .base_high = 0,
        },

        /*TODO: Kernel Code Descriptor*/
        {
            .segment_low = 0xFFFF,
            .base_low = 0,
            // Next 16-bit (Bit 32 to 07)
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            // TODO= Continue SegmentDescriptor definition
            .dpl_bit = 0,
            .segment_present = 1,
            .segment_high = 0xF,
            .avail_bit = 0,
            .l_flag = 0, 
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,
        },

        /*TODO: Kernel Data Descriptor*/
        {
            .segment_low = 0xFFFF, 
            .base_low = 0,

            // Next 16-bit (Bit 32 to 07)
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            // TODO= Continue SegmentDescriptor definition
            .dpl_bit = 0,
            .segment_present = 1,
            .segment_high = 0xF,
            .avail_bit = 0,
            .l_flag = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0,

            // TODO : Implement
        },

        // /*TODO: User Code Descriptor*/
        {
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .dpl_bit = 0x3,
            .segment_present = 1,
            .segment_high = 0xF,
            .avail_bit = 0,
            .l_flag = 0, // beda
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0},

        /*TODO: User Data Descriptor*/
        {
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            .dpl_bit = 0x3,
            .segment_present = 1,
            .segment_high = 0xF,
            .avail_bit = 0,
            .l_flag = 0,
            .db_flag = 1,
            .granularity = 1,
            .base_high = 0},

        {
            .segment_high = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
            .segment_low = sizeof(struct TSSEntry),
            .base_high = 0,
            .base_mid = 0,
            .base_low = 0,
            .non_system = 0, // S bit
            .type_bit = 0x9,
            .dpl_bit = 0,   // DPL
            .segment_present = 1,   // P bit
            .db_flag = 1,  // D/B bit
            .l_flag = 0,   // L bit
            .granularity = 0, // G bit
        },
        {0}

    }};

void gdt_install_tss(void)
{
    uint32_t base = (uint32_t)&_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low = base & 0xFFFF;
}

/**
 * _gdt_gdtr, predefined system GDTR.
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    .size = sizeof(global_descriptor_table) - 1,
    .address = &global_descriptor_table,
    // TODO : Implement, this GDTR will point to global_descriptor_table.
    //        Use sizeof operator
};
