#include "paging.h"

// 4KB aligned global page directory and one page table
struct page_directory_entry pd[1024] __attribute__((aligned(4096)));
struct page pt[1024] __attribute__((aligned(4096)));

// Map a linked list of physical pages to a virtual address
void *map_pages(void *vaddr, struct ppage *pglist, struct page_directory_entry *pd) {
    uint32_t addr = (uint32_t)vaddr;
    int pd_index = addr >> 22;           // top 10 bits
    int pt_index = (addr >> 12) & 0x3FF; // next 10 bits

    // Initialize page directory entry if not present
    if (!pd[pd_index].present) {
        pd[pd_index].present = 1;
        pd[pd_index].rw = 1;
        pd[pd_index].frame = ((uint32_t)pt) >> 12;
    }

    struct page *page_table = (struct page *)(pd[pd_index].frame << 12);
    struct ppage *pg = pglist;
    while (pg) {
        page_table[pt_index].present = 1;
        page_table[pt_index].rw = 1;
        page_table[pt_index].frame = ((uint32_t)pg->physical_addr) >> 12;
        pg = pg->next;
        pt_index++;
    }
    return vaddr;
}

// Load page directory into CR3
void loadPageDirectory(struct page_directory_entry *pd) {
    __asm__ __volatile__("mov %0,%%cr3" : : "r"(pd) : );
}

// Enable paging by setting CR0 bits
void enable_paging(void) {
    __asm__ __volatile__(
        "mov %%cr0, %%eax\n"
        "or $0x80000001, %%eax\n"
        "mov %%eax, %%cr0"
        : : : "eax"
    );
}

