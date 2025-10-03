
#include <stdint.h>
#include "rprintf.h"  // for esp_printf

#define MULTIBOOT2_HEADER_MAGIC         0xe85250d6

const unsigned int multiboot_header[]  __attribute__((section(".multiboot"))) = {MULTIBOOT2_HEADER_MAGIC, 0, 16, -(16+MULTIBOOT2_HEADER_MAGIC), 0, 12};

uint8_t inb (uint16_t _port) {
    uint8_t rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VGA_MEMORY 0xB8000
#define DEFAULT_COLOR 7

unsigned char keyboard_map[128] =
{
   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
 '9', '0', '-', '=', '\b',     /* Backspace */
 '\t',                 /* Tab */
 'q', 'w', 'e', 'r',   /* 19 */
 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
   0,                  /* 29   - Control */
 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
'\'', '`',   0,                /* Left shift */
'\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
 'm', ',', '.', '/',   0,                              /* Right shift */
 '*',
   0,  /* Alt */
 ' ',  /* Space bar */
   0,  /* Caps lock */
   0,  /* 59 - F1 key ... > */
   0,   0,   0,   0,   0,   0,   0,   0,  
   0,  /* < ... F10 */
   0,  /* 69 - Num lock*/
   0,  /* Scroll Lock */
   0,  /* Home key */
   0,  /* Up Arrow */
   0,  /* Page Up */
 '-',
   0,  /* Left Arrow */
   0,  
   0,  /* Right Arrow */
 '+',
   0,  /* 79 - End key*/
   0,  /* Down Arrow */
   0,  /* Page Down */
   0,  /* Insert Key */
   0,  /* Delete Key */
   0,   0,   0,  
   0,  /* F11 Key */
   0,  /* F12 Key */
   0,  /* All other keys are undefined */
};

struct termbuf {
    char ascii;
    char color;
};

static int x = 0;  // column
static int y = 0;  // row

/* Scroll screen up by one row */
void scroll_up(void) {
    struct termbuf *vram = (struct termbuf*)VGA_MEMORY;

    // Move each row up
    for (int row = 1; row < SCREEN_HEIGHT; row++)
        for (int col = 0; col < SCREEN_WIDTH; col++)
            vram[(row-1)*SCREEN_WIDTH + col] = vram[row*SCREEN_WIDTH + col];

    // Clear last row
    for (int col = 0; col < SCREEN_WIDTH; col++) {
        vram[(SCREEN_HEIGHT-1)*SCREEN_WIDTH + col].ascii = ' ';
        vram[(SCREEN_HEIGHT-1)*SCREEN_WIDTH + col].color = DEFAULT_COLOR;
    }
}

/* Write a single character sequentially to the terminal */
int putc(int c) {
    struct termbuf *vram = (struct termbuf*)VGA_MEMORY;

    if (c == '\n') { x = 0; y++; }
    else if (c == '\r') { x = 0; }
    else {
        vram[y * SCREEN_WIDTH + x].ascii = c;
        vram[y * SCREEN_WIDTH + x].color = DEFAULT_COLOR;
        x++;
    }

    if (x >= SCREEN_WIDTH) {
    	x = 0;
	y++;
	}

    /* Scroll if needed */
    if (y >= SCREEN_HEIGHT) {
        scroll_up();
        y = SCREEN_HEIGHT - 1;
    }
	return c;
}

void main() {
    // Print current execution level
    esp_printf(putc, "Current execution level: Kernel mode (Ring 0)\r\n");

    // Print multiple lines to demonstrate scrolling
    for (int i = 1; i <= 30; i++) {
        esp_printf(putc, "Line %d: The quick brown fox jumps over the lazy dog.\r\n", i);
    }

    // Example formatted output
    esp_printf(putc, "Decimal: %d  Hex: %x  Char: %c  String: %s\r\n",
               12345, 0xBEEF, 'A', "Hello esp_printf!");

   esp_printf(putc, "\n"); 	

while(1) {
    // Read the PS/2 controller status register at port 0x64
    	uint8_t status = inb(0x64);

    // Check if the output buffer is full (LSB = 1)
    // If LSB is set, a new keyboard scancode is available
   	if(status & 1) {
        // Read the scancode from the PS/2 data port (0x60)
        uint8_t scancode = inb(0x60);

    // Ignore invalid scancodes that are out of bounds for our mapping table
        if (scancode >= 128) {
            continue;
        }

        // Print the scancode in hexadecimal and the corresponding key from the keyboard map
        esp_printf(putc, "%x=%c | ", scancode, keyboard_map[scancode]);
    }
}

}
