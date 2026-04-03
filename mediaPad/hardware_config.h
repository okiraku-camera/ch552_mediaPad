// meaiaPad.ino main sketch of MediaPad program using ch552g.
// Copyright (c)  Takeshi Higasa, okiraku-camera.tokyo
// This software is released under the MIT License.
// http://opensource.org/licenses/mit-license.php   

// Hardware dependent constants are defined in this file, such as the number of rows and columns, the eeprom memory map, etc. 
// These constants are used in multiple files, so they are defined in a separate header file.    

#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// EEPROM memory map

#define EEPROM_START    0   // start address of EEPROM
#define EEPROM_END      127  // end address of EEPROM. Do not write beyond this.

#define COLS 2
#define ROWS 4
#define NUM_KEYS (COLS * ROWS)
#define NUM_LAYERS 2
#define RE_COUNT 1	// No RE

#define KEYMAP_START	2   // programable keymap starts at EEPROM_START. After EEPROM_KEYMAP_VALID1 and 2.
#define KEYMAP_SIZE     32  // (NUM_KEYS * NUM_LAYERS * 2) // size of keymap. 2bytes a key.    
#define RE_KEYMAP_START 		34        // Start address of RE data. 
// RE that can be pressed is treated as a regular   key switch.  

#define RE_KEYMAP_END            (RE_KEYMAP_START + RE_COUNT * NUM_LAYERS * 4)	// end of RE data.

#define MACRO_START	    42  //  macros start after RE keymap.
#define MACRO_SIZE	    85  // (MACRO_END - MACRO_START)  	  // size of macro storage in bytes.
#define MACRO_END       127 // (EEPROM_END)					  // end of macro storage. Do not write beyond this.

// -----------------------------------------------------------------------------
// Encoder data layout in NVM keymap area
// -----------------------------------------------------------------------------
// - RE keymap starts at RE_KEYMAP_START.
// - Data is grouped by layer.
// - One encoder consumes 4 bytes per layer:
//     [CW low][CW high][CCW low][CCW high]
// - Address for encoder 're' in 'layer':
//     RE_KEYMAP_START + (layer * RE_COUNT * 4) + (re * 4)
// - read_re_keys()/write_re_keys() use uint16_t buffer in CW, CCW order:
//     buffer[re*2 + 0] = CW, buffer[re*2 + 1] = CCW

// -----------------------------------------------------------------------------
// EEPROM/NVM address map (address increases downward)
// -----------------------------------------------------------------------------
//   0x00  EEPROM_START
//   0x00-0x01 : NVM keymap valid magic (0xBEEF)
//   0x02  KEYMAP_START
//   0x02-0x21 : Keymap area (KEYMAP_SIZE = 32 bytes)
//   0x22  RE_KEYMAP_START
//   0x22-0x29 : RE keymap area (RE_COUNT * NUM_LAYERS * 4 bytes)
//               per encoder: [CW low][CW high][CCW low][CCW high]
//   0x2A  MACRO_START
//   0x2A-0x7E : Macro data area (MACRO_SIZE = 85 bytes)
//   0x7F  MACRO_END (macro terminator location)
//   0x7F  EEPROM_END

#endif  // HARDWARE_CONFIG_H