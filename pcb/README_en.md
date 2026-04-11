# PCB (ch552_mediaPad)

This directory contains KiCad design data (schematics and PCB layout) for CH552 MediaPad.

## Tool

- KiCad 8.0

## Main Files

- `ch552_mediaPad.kicad_sch` : Schematic
- `ch552_mediaPad.kicad_pcb` : PCB layout
- `ch552_mediaPad.kicad_pro` : Project settings
- `local/` : Footprint library for this PCB

### Contents of local/

- `Hole_M3_SmallPad.kicad_mod` : M3 mounting hole
- `MX_PCB_1.25u.kicad_mod` : Footprint for 1.25u MX switch
- `MX_Plate_hole.kicad_mod` : Footprint used to design a switch plate by replacing the MX switch footprint
- `SW_Cherry_MX_PCB_RE_1.25u.kicad_mod` : Combined footprint for MX 1.25U and rotary encoder EC-12
- `USB_C_Receptacle_304J-BCP16.kicad_mod` : USB Type-C connector

## Notes

- When collaborating, check diffs in related project setting files along with schematic/PCB edits.
