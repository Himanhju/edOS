#include <Graphics/VESA/VESA.h>

#include "Paging.h"

dword* const page_directory = (dword*) PAGE_DIR_BASE;
dword* const page_tables = (dword*) PAGE_TABLES_BASE; // Flat 1024 * 1024

void Page_Init(void) {
    pstr_8x8("Enabling Paging\n", Get_RGB(0xffffff));
    for (unsigned int i = 0; i < 1024; i++) {
        dword* table = page_tables + i * 1024; // Each page table is 4KB (1024 dwords)

        for (unsigned int j = 0; j < 1024; j++) {
            dword phys_addr = ((i * 1024 + j) * 0x1000); // Identity map
            table[j] = phys_addr | PAGE_PRESENT | PAGE_RW;
        }

        page_directory[i] = ((dword)table & 0xFFFFF000) | PAGE_PRESENT | PAGE_RW;
    }

    asm volatile ("mov %0, %%cr3" :: "r" (PAGE_DIR_BASE));
    asm volatile (
        "mov %%cr0, %%eax\n"
        "or $0x80000000, %%eax\n"
        "mov %%eax, %%cr0"
        :
        :
        : "eax", "memory"
    );
    pstr_8x8("Paging Enabled Successfully\n\n", Get_RGB(0x7CFC00));
}

void Remap_Page(dword virtual_addr, dword physical_addr, dword flags) {
    dword pd_index = (virtual_addr >> 22) & 0x3FF; // Top 10 bits
    dword pt_index = (virtual_addr >> 12) & 0x3FF; // Next 10 bits

    // Ensure the page table is present
    if (!(page_directory[pd_index] & PAGE_PRESENT)) {
        // You can add dynamic page table allocation here if needed
        return; // Page table not present — can't remap
    }

    dword* page_table = (dword*)(page_directory[pd_index] & 0xFFFFF000);
    page_table[pt_index] = (physical_addr & 0xFFFFF000) | (flags & 0xFFF);

    // Invalidate the TLB for the specific virtual address
    asm volatile ("invlpg (%0)" :: "r"(virtual_addr) : "memory");
}

//flush tlb:
//  dword addr = 0x00100000;
//  asm volatile ("invlpg (%0)" :: "r"(addr) : "memory");
