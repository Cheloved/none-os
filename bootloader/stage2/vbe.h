#pragma once
#include <stdint.h>
#include "stdio.h"

typedef struct
{
    // Offset  Size    Description     (Table 00077)
    // 00h  4 BYTEs   (ret) signature ("VESA")
    uint8_t signature[4];
    //     (call) VESA 2.0 request signature ("VBE2"), required to receive
    // version 2.0 info
    // 04h    WORD    VESA version number (one-digit minor version -- 0102h = v1.2)
    uint16_t vesa_version;
    // 06h    DWORD   pointer to OEM name
    uint32_t oem_ptr;
    //     "761295520" for ATI
    // 0Ah    DWORD   capabilities flags (see #00078)
    uint32_t cap_flags;
    // 0Eh    DWORD   pointer to list of supported VESA and OEM video modes
    uint32_t video_modes_ptr;
    //     (list of words terminated with FFFFh)
    // 12h    WORD    total amount of video memory in 64K blocks
    uint16_t memory_size;
    //     ---VBE v1.x ---
    // 14h 236 BYTEs  reserved
    uint8_t reserved[236];
    //     ---VBE v2.0 ---
    // 14h    WORD    OEM software version (BCD, high byte = major, low byte = minor)
    uint16_t oem_version;
    // 16h    DWORD   pointer to vendor name
    uint32_t vendor_name_ptr;
    // 1Ah    DWORD   pointer to product name
    uint32_t product_name_ptr;
    // 1Eh    DWORD   pointer to product revision string
    uint32_t product_revision;
    // 22h    WORD    (if capabilities bit 3 set) VBE/AF version (BCD)
    uint16_t vbe_af_version;
    //     0100h for v1.0P
    // 24h    DWORD   (if capabilities bit 3 set) pointer to list of supported
    uint32_t accel_modes_ptr;
    //     accelerated video modes (list of words terminated with FFFFh)
    // 28h 216 BYTEs  reserved for VBE implementation
    uint8_t reserved_impl[216];
    // 100h 256 BYTEs  OEM scratchpad (for OEM strings, etc.) 
    uint8_t oem_scratchpad[256];
} __attribute__((packed)) VBEInfo;

typedef struct
{
    // Offset  Size    Description     (Table 00079)
    // 00h    WORD    mode attributes (see #00080)
    uint16_t attributes;
    // 02h    BYTE    window attributes, window A (see #00081)
    uint8_t  window_a_attr;
    // 03h    BYTE    window attributes, window B (see #00081)
    uint8_t  window_b_attr;
    // 04h    WORD    window granularity in KB
    uint16_t window_granularity;
    // 06h    WORD    window size in KB
    uint16_t window_size;
    // 08h    WORD    start segment of window A (0000h if not supported)
    uint16_t window_a_start;
    // 0Ah    WORD    start segment of window B (0000h if not supported)
    uint16_t window_b_start;
    // 0Ch    DWORD   -> FAR window positioning function (equivalent to AX=4F05h)
    uint32_t far_window_pos_func;
    // 10h    WORD    bytes per scan line
    uint16_t bytes_per_scan_line;
    //     ---remainder is optional for VESA modes in v1.0/1.1, needed for OEM modes---
    // 12h    WORD    width in pixels (graphics) or characters (text)
    uint16_t width;
    // 14h    WORD    height in pixels (graphics) or characters (text)
    uint16_t height;
    // 16h    BYTE    width of character cell in pixels
    uint8_t  char_width;
    // 17h    BYTE    height of character cell in pixels
    uint8_t  char_height;
    // 18h    BYTE    number of memory planes
    uint8_t  num_memory_planes;
    // 19h    BYTE    number of bits per pixel
    uint8_t  bits_per_pixel;
    // 1Ah    BYTE    number of banks
    uint8_t  num_memory_banks;
    // 1Bh    BYTE    memory model type (see #00082)
    uint8_t  mem_model_type;
    // 1Ch    BYTE    size of bank in KB
    uint8_t  bank_size;
    // 1Dh    BYTE    number of image pages (less one) that will fit in video RAM
    uint8_t  num_image_pages;
    // 1Eh    BYTE    reserved (00h for VBE 1.0-2.0, 01h for VBE 3.0)
    uint8_t  reserved;
    //     ---VBE v1.2+ ---
    // 1Fh    BYTE    red mask size
    // 20h    BYTE    red field position
    // 21h    BYTE    green mask size
    // 22h    BYTE    green field size
    // 23h    BYTE    blue mask size
    // 24h    BYTE    blue field size
    // 25h    BYTE    reserved mask size
    // 26h    BYTE    reserved mask position
    // 27h    BYTE    direct color mode info
    uint8_t mask_stuff[9];
    //     bit 0:
    //     Color ramp is programmable
    //     bit 1:
    //     Bytes in reserved field may be used by application
    //     ---VBE v2.0+ ---
    // 28h    DWORD   physical address of linear video buffer
    uint32_t linear_buffer_addr;
    // 2Ch    DWORD   pointer to start of offscreen memory
    uint32_t offscreen_ptr;
    // 30h    WORD    KB of offscreen memory
    uint16_t offscreen_memory;
    //     ---VBE v3.0 ---
    // 32h    WORD    bytes per scan line in linear modes
    uint16_t bytes_per_scan_ling;
    // 34h    BYTE    number of images (less one) for banked video modes
    uint8_t  images_banked;
    // 35h    BYTE    number of images (less one) for linear video modes
    uint8_t  images_linear;
    // 36h    BYTE    linear modes:
    //     Size of direct color red mask (in bits)
    // 37h    BYTE    linear modes:
    //     Bit position of red mask LSB (e.g. shift count)
    // 38h    BYTE    linear modes:
    //     Size of direct color green mask (in bits)
    // 39h    BYTE    linear modes:
    //     Bit position of green mask LSB (e.g. shift count)
    // 3Ah    BYTE    linear modes:
    //     Size of direct color blue mask (in bits)
    // 3Bh    BYTE    linear modes:
    //     Bit position of blue mask LSB (e.g. shift count)
    // 3Ch    BYTE    linear modes:
    //     Size of direct color reserved mask (in bits)
    // 3Dh    BYTE    linear modes:
    //     Bit position of reserved mask LSB
    uint8_t  linear_mask_stuff[8];
    // 3Eh    DWORD   maximum pixel clock for graphics video mode, in Hz
    uint32_t max_pixel_clock;
    // 42h 190 BYTEs  reserved (0)
    uint8_t reserved0[190];
} __attribute__((packed)) VBEModeInfo;

typedef struct
{
    uint16_t number;
    uint16_t width;
    uint16_t height;
    uint8_t  bpp;
    uint32_t* vmem;
} VideoMode;

extern VBEModeInfo global_selected_vbemode;
extern VideoMode   selected_mode;

uint16_t read_vbeinfo(VBEInfo* vbeinfo);
uint16_t read_mode_info(uint16_t number, VBEModeInfo* modeinfo);
uint16_t select_mode(uint16_t number);
uint8_t user_select_mode();

uint16_t get_far_value(uint32_t ptr);
