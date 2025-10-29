#include <stdint.h>
#include "rprintf.h"
#include "page.h"
#include "paging.h"

#define VGA_MEMORY      0xB8000
#define SCREEN_WIDTH    80
#define SCREEN_HEIGHT   25
#define VGA_COLOR       0x07
#define MULTIBOOT2_HEADER_MAGIC 0xe85250d6

extern char _end_kernel;   // linker symbol for the end of the kernel

// Multiboot header
const unsigned int multiboot_header[] __attribute__((section(".multiboot"))) = {
    MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16 + MULTIBOOT2_HEADER_MAGIC), 0, 12
};

// Cursor position
static int cursor_row = 0;
static int cursor_col = 0;

// ===== Terminal output =====
int kputc(int data) {
    volatile uint16_t *vram = (uint16_t*)VGA_MEMORY;

    if (data == '\n') { 
        cursor_col = 0; 
        cursor_row++; 
    } else if (data == '\r') { 
        cursor_col = 0; 
    } else {
        int pos = cursor_row * SCREEN_WIDTH + cursor_col;
        vram[pos] = (VGA_COLOR << 8) | (uint8_t)data;
        cursor_col++;
        if (cursor_col >= SCREEN_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    // Scroll screen if needed
    if (cursor_row >= SCREEN_HEIGHT) {
        for (int r = 1; r < SCREEN_HEIGHT; r++)
            for (int c = 0; c < SCREEN_WIDTH; c++)
                vram[(r-1) * SCREEN_WIDTH + c] = vram[r * SCREEN_WIDTH + c];
        for (int c = 0; c < SCREEN_WIDTH; c++)
            vram[(SCREEN_HEIGHT-1) * SCREEN_WIDTH + c] = (VGA_COLOR << 8) | ' ';
        cursor_row = SCREEN_HEIGHT - 1;
        cursor_col = 0;
    }

    return data;
}

// ===== Kernel main =====
void main() {
/*    // Initialize interrupts
    remap_pic();
    load_gdt();
    init_idt();
    esp_printf(kputc, "Initializing interrupts...\n");
    asm("sti");  // Enable interrupts

*/
    esp_printf(kputc, "Kernel initialized.\n");

    // Execution level
    esp_printf(kputc, "Current execution level: %d\n", 0);

    // Initialize page frame allocator (for general allocations)
    init_pfa_list();

    // Test allocation and free
    struct ppage *alloc = allocate_physical_pages(3);
    free_physical_pages(alloc);

    // ===== Identity map kernel pages =====
  
    for (uint32_t va = 0x100000; va < (uint32_t)&_end_kernel; va += 0x1000) {
        struct ppage tmp;
        tmp.physical_addr = (void*)va;   // cast integer to void*
        tmp.next = NULL;
        map_pages((void*)va, &tmp, pd);
    }

    // ===== Map stack page =====
    uint32_t esp;
    asm("mov %%esp,%0" : "=r"(esp));
    struct ppage stack_page;
    stack_page.physical_addr = (void*)(esp & 0xFFFFF000); // align to 4KB, cast
    stack_page.next = NULL;
    map_pages((void*)(esp & 0xFFFFF000), &stack_page, pd);

    // ===== Map VGA video memory =====
    struct ppage vga_page;
    vga_page.physical_addr = (void*)VGA_MEMORY; // cast to void*
    vga_page.next = NULL;
    map_pages((void*)VGA_MEMORY, &vga_page, pd);

    // Load page directory and enable paging
    loadPageDirectory(pd);
    enable_paging();

    // Main idle loop
    while (1) { }
}
