# CH552 MediaPad

日本語版: [README.md](README.md)

A USB HID macro pad featuring 8 keys (2x4 layout) + rotary encoder, powered by the CH552G microcontroller. Real-time keymap configuration via WebHID.

## Directory Structure

- **`pcb/`**: KiCAD 8 PCB design (Schematics and PCB files)
- **`plates/`**: Laser-cut acrylic plate designs
- **`mediaPad/`**: Arduino firmware for CH552G
- **`html/`**: WebHID browser application (HTML/CSS/JavaScript)
- **`LICENSE`**: Project license

## Hardware

- **Microcontroller**: CH552G
- **Keys**: 8 keys (2x4 layout)
- **Encoder**: 1x rotary encoder
- **Top Plate**: 5mm acrylic (transparent)
- **Middle Plate**: 5mm acrylic (matte transparent)
- **Bottom Plate**: 2mm acrylic (transparent)
- **USB**: USB Type-C connector

See [pcb/README.md](pcb/README.md) and [plates/README.md](plates/README.md) for details.

## Software

### Firmware (mediaPad/)
Arduino sketch for CH552G. Supports USB HID keyboard input, media controls, and rotary encoder input.

### Web App (html/)
Browser-based application that communicates with the device via **WebHID** for reading/writing keymap configuration.

## License

This project contains hardware and software components.

Copyright (c) Takeshi Higasa, okiraku-camera.tokyo

See [LICENSE](./LICENSE) for details.



