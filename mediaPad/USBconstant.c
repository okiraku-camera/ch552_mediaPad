#include "USBconstant.h"
/**
 * USBconstant.c USB descriptors and HID report descriptor of ch552 based keyboard firmware.
 * from CH55xDuino generic_examples.
 */
// Device descriptor
__code USB_Descriptor_Device_t DeviceDescriptor = {
	.Header = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},
	.USBSpecification = VERSION_BCD(1, 1, 0),
	.Class = 0x00,
	.SubClass = 0x00,
	.Protocol = 0x00,
	.Endpoint0Size = DEFAULT_ENDP0_SIZE,
	.VendorID = 0x1209,
	.ProductID = 0xc55D,
	.ReleaseNumber = VERSION_BCD(1, 0, 0),
	.ManufacturerStrIndex = 1,
	.ProductStrIndex = 2,
	.SerialNumStrIndex = 3,
	.NumberOfConfigurations = 1};

/** Configuration descriptor structure. This descriptor, located in FLASH
 * memory, describes the usage of the device in one of its supported
 * configurations, including information about any device interfaces and
 * endpoints. The descriptor is read out by the USB host during the enumeration
 * process when selecting a configuration so that the host may correctly
 * communicate with the USB device.
 */
__code USB_Descriptor_Configuration_t ConfigurationDescriptor = {
	.Config = 
			{.Header = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), 
									.Type = DTYPE_Configuration},
			.TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
			.TotalInterfaces = 1,
			.ConfigurationNumber = 1,
			.ConfigurationStrIndex = NO_DESCRIPTOR,
			.ConfigAttributes = (USB_CONFIG_ATTR_RESERVED),
			.MaxPowerConsumption = USB_CONFIG_POWER_MA(50)},
	.HID_Interface = 
			{.Header = {.Size = sizeof(USB_Descriptor_Interface_t),
									.Type = DTYPE_Interface},
			.InterfaceNumber = 0,
			.AlternateSetting = 0x00,
			.TotalEndpoints = 2,
			.Class = HID_CSCP_HIDClass,
			.SubClass = HID_CSCP_BootSubclass,
			.Protocol = HID_CSCP_KeyboardBootProtocol,
			.InterfaceStrIndex = NO_DESCRIPTOR},

	.HID_KeyboardHID = 
			{.Header = {.Size = sizeof(USB_HID_Descriptor_HID_t),
									.Type = HID_DTYPE_HID},
			.HIDSpec = VERSION_BCD(1, 1, 0),
			.CountryCode = 0x00,
			.TotalReportDescriptors = 1,
			.HIDReportType = HID_DTYPE_Report,
			.HIDReportLength = sizeof(ReportDescriptor)},

	.HID_ReportINEndpoint = 
			{.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
									.Type = DTYPE_Endpoint},
			.EndpointAddress = EPADDRESS_IN,
			.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize = EPSIZE,
			.PollingIntervalMS = 10},
			
	.HID_ReportOUTEndpoint = 
			{.Header = {.Size = sizeof(USB_Descriptor_Endpoint_t),
									.Type = DTYPE_Endpoint},
			.EndpointAddress = EPADDRESS_OUT,
			.Attributes = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
			.EndpointSize = EPSIZE,
			.PollingIntervalMS = 10},
};  // ConfigurationDescriptor

// HID report descriptors 
__code uint8_t ReportDescriptor[] = {
	0x05, 0x01, // USAGE_PAGE (Generic Desktop)
	0x09, 0x06, // USAGE (Keyboard)
	0xa1, 0x01, // COLLECTION (Application)
	0x85, REPORT_ID_KBD, //   REPORT_ID 
// Modifiers
	0x05, 0x07, //   USAGE_PAGE (Keyboard)
	0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x01, //   LOGICAL_MAXIMUM (1)
	0x95, 0x08, //   REPORT_COUNT (8)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x81, 0x02, //   INPUT (Data,Var,Abs)
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x81, 0x03, //   INPUT (Cnst,Var,Abs)
// Keyboard
	0x05, 0x07, //   USAGE_PAGE (Keyboard)
	0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
	0x2a, 0xff, 0x00,	//   USAGE_MAXIMUM (0x00ff)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (0x00ff)
	0x95, 0x06, //   REPORT_COUNT (6)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x81, 0x00, //   INPUT (Data,Ary,Abs)
// Keyboard LED
	0x05, 0x08, //   USAGE_PAGE (LEDs)
	0x19, 0x01, //   USAGE_MINIMUM (Num Lock)
	0x29, 0x05, //   USAGE_MAXIMUM (Kana)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x25, 0x01, //   LOGICAL_MAXIMUM (1)
	0x95, 0x05, //   REPORT_COUNT (5)
	0x75, 0x01, //   REPORT_SIZE (1)
	0x91, 0x02, //   OUTPUT (Data,Var,Abs)
	0x95, 0x01, //   REPORT_COUNT (1)
	0x75, 0x03, //   REPORT_SIZE (3)
	0x91, 0x03, //   OUTPUT (Cnst,Var,Abs)
	0xc0,        // END_COLLECTION
// 3button mouse with wheel.
	0x05, 0x01,							// USAGE_PAGE (Generic Desktop)  // 54
	0x09, 0x02,							// USAGE (Mouse)
	0xa1, 0x01,							// COLLECTION (Application)
	0x09, 0x01,							//   USAGE (Pointer)
	0xa1, 0x00,							//   COLLECTION (Physical)
  0x85, REPORT_ID_MOUSE,	//     REPORT_ID
	//	button. (3 buttons)
	0x05, 0x09,							//     USAGE_PAGE (Button)
	0x19, 0x01,							//     USAGE_MINIMUM (Button 1)
	0x29, 0x03,							//     USAGE_MAXIMUM (Button 3)
	0x15, 0x00,							//     LOGICAL_MINIMUM (0)
	0x25, 0x01,							//     LOGICAL_MAXIMUM (1)
	0x95, 0x03,							//     REPORT_COUNT (3)
	0x75, 0x01,							//     REPORT_SIZE (1)
	0x81, 0x02,							//     INPUT (Data,Var,Abs)
	0x95, 0x01,							//     REPORT_COUNT (1)
	0x75, 0x05,							//     REPORT_SIZE (5)
	0x81, 0x03,							//     INPUT (Cnst,Var,Abs)
// x, y, wheel
	0x05, 0x01,							//     USAGE_PAGE (Generic Desktop)
	0x09, 0x30,							//     USAGE (X)
	0x09, 0x31,							//     USAGE (Y)
	0x09, 0x38,							//     USAGE (Wheel)
	0x15, 0x81,							//     LOGICAL_MINIMUM (-127)
	0x25, 0x7f,							//     LOGICAL_MAXIMUM (127)
	0x75, 0x08,							//     REPORT_SIZE (8)
	0x95, 0x03,							//     REPORT_COUNT (3)
	0x81, 0x06,							//     INPUT (Data,Var,Rel)
	0xc0,										//     END_COLLECTION
	0xc0,										//   END_COLLECTION
// Consumer controls.
  0x05, 0x0c, // USAGE_PAGE (Consumer device)
  0x09, 0x01, // USAGE (Consumer Control)
  0xa1, 0x01, 
  0x85, REPORT_ID_CONSUMER, 
  0x05, 0x0c,
  0x75, 0x10,  // REPORT SIZE = 16bit.
  0x95, 0x01, // REPORT COUNT = 1
  0x15, 0x00, // LOGICAL MIN = 0
  0x26, 0xff, 0x03, // LOGICAL MAX = 0x3ff (limited code range.)
  0x19, 0x00, // USAGE MIN = 0
  0x2a, 0xff, 0x03, // USAGE MAX = 0x3ff  
  0x81, 0x00, // INPUT (Data,Ary,Abs)
  0xc0, 
// System control .
	0x05, 0x01, // GENERIC Desktop
	0x09, 0x80, // SYSTEM Control.
	0xA1, 0x01, 
	0x85, REPORT_ID_SYSTEM,
	0x09, 0x81, // System PowerDown
	0x09, 0x82, // System SleepKEYBOARD_EPSIZE
	0x09, 0x83, // System WakeUp.
	0x15, 0x00, // LOGICAL_MINIMUM (0) 
	0x25, 0x01, // LOGICAL_MAXIMUM (1) 
	0x75, 0x01, // REPORT_SIZE  1bit.
	0x95, 0x03, // REPORT_COUNT 3 items.
	0x81, 0x02, // INPUT (Data,Var,Abs) 
	0x95, 0x0d, // REPORT_COUNT (filler)
	0x81, 0x01, // INPUT (Data,Const, Abs) 
	0xc0, // END_COLLECTION

// vender defined device for webhid
	0x06, 0x82, 0xff,	// Usage Page 0xff82
	0x09, 0x51,				// Usage 0x51
	0xa1, 0x01,
	0x85, REPORT_ID_RAW,

	0x09, 0x52,				// Usage 0x52
	0x19, 0x00, //   USAGE_MINIMUM 
	0x2a, 0xff, 0x00,	//   USAGE_MAXIMUM (0x00ff)
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (0x00ff)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x95, RAW_DATA_SIZE, //   REPORT_COUNT (RAW_DATA_SIZE)
	0x91, 0x00, //	OUTPUT (Data,Array,Abs) 

	0x09, 0x53,				// Usage 0x53
	0x15, 0x00, //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (0x00ff)
	0x75, 0x08, //   REPORT_SIZE (8)
	0x95, RAW_DATA_SIZE, //   REPORT_COUNT (RAW_DATA_SIZE)
 	0x81, 0x00, //	INPUT (Data,Ary,Abs)
	0xc0 // END_COLLECTION
};

// String Descriptors
__code uint8_t LanguageDescriptor[] = {0x04, 0x03, 0x09, 0x04}; // Language Descriptor
__code uint16_t SerialDescriptor[] = {	// Serial String Descriptor
	(((9 + 1) * 2) | (DTYPE_String << 8)),
	'C','H','5','5','2',' ','k','b','d'};
__code uint16_t ProductDescriptor[] = {	// Produce String Descriptor
	(((14 + 1) * 2) | (DTYPE_String << 8)),
	'C','H','5','5','2',' ','m','e','d','i','a','P','a','d'};
__code uint16_t ManufacturerDescriptor[] = { // SDCC is little endian
	(((14 + 1) * 2) | (DTYPE_String << 8)), 
	'o','k','i','r','a','k','u','-','c','a','m','e','r','a'};
