#include "header/cpu/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
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
        {
            .segment_low = 0,
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
        {
            .segment_low = 0,
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
        }}};

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
