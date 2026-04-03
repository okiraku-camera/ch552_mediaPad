#ifndef __USB_CONST_DATA_H__
#define __USB_CONST_DATA_H__
/**
 * USBconstant.h USB descriptors and HID report descriptor definitions of ch552 based macro keypad firmware.
 * from CH55xDuino generic_examples.
 */

// clang-format off
#include <stdint.h>
#include "include/ch5xx.h"
#include "include/ch5xx_usb.h"
#include "usbCommonDescriptors/StdDescriptors.h"
#include "usbCommonDescriptors/HIDClassCommon.h"
// clang-format on

#define EP0_ADDR 0
#define EP1_ADDR 10

// use EP1
#define EPADDRESS_IN	0x81
#define EPADDRESS_OUT	1
// standard EP size.
#define EPSIZE 64
#define REPORT_IN_START_INDEX		64
#define REPORT_OUT_START_INDEX	0

#define RAW_DATA_SIZE	32

#define REPORT_ID_KBD				1
#define REPORT_ID_MOUSE			2
#define REPORT_ID_CONSUMER	3
#define REPORT_ID_SYSTEM		4
#define REPORT_ID_RAW			5


/** Type define for the device configuration descriptor structure. This must be
 * defined in the application code, as the configuration descriptor contains
 * several sub-descriptors which vary between devices, and which describe the
 * device's usage to the host.
 */
typedef struct {
  USB_Descriptor_Configuration_Header_t Config;

  // Keyboard HID Interface
  USB_Descriptor_Interface_t HID_Interface;
  USB_HID_Descriptor_HID_t HID_KeyboardHID;
  USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
  USB_Descriptor_Endpoint_t HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

extern __code USB_Descriptor_Device_t DeviceDescriptor;
extern __code USB_Descriptor_Configuration_t ConfigurationDescriptor;
extern __code uint8_t ReportDescriptor[];
extern __code uint8_t LanguageDescriptor[];
extern __code uint16_t SerialDescriptor[];
extern __code uint16_t ProductDescriptor[];
extern __code uint16_t ManufacturerDescriptor[];

#endif
