#pragma once

// See MultiProcessor Specification Version 1.[14]

struct mp {              // floating pointer
    uchar signature[4];  // "_MP_"
    void* physaddr;      // phys addr of MP config table
    uchar length;        // 1
    uchar specrev;       // [14]
    uchar checksum;      // all bytes must add up to 0
    uchar type;          // MP system config type
    uchar imcrp;
    uchar reserved[3];
};

struct rsdp {
    uchar signature[8];
    uchar checksum;
    uchar oemid[6];
    uchar revision;
    uint rsdtaddress;
    // since version 2.0
    uint length;
    uint xsdtaddress[2];
    uchar extendedchecksum;
    uchar reserved[3];
};

struct acpisdthdr {
    uchar signature[4];
    uint length;
    uchar revision;
    uchar checksum;
    uchar oemid[6];
    uchar oemtableid[8];
    uint oemrevision;
    uint creatorid;
    uint creatorrevision;
};

struct xsdt {
    struct acpisdthdr header;
    uint table[0][2];
};

struct rsdt {
    struct acpisdthdr header;
    uint table[0];
};

struct madt {
    struct acpisdthdr header;
    uint lca;
    uint flags;
    struct madte {
        uchar type;
        uchar length;
        uchar acpiprocessorid;  // for local APIC
        uchar apicid;           // for local APIC
        uint flags;             // for local APIC
    } table[0];
};

struct ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

struct GenericAddressStructure {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
};

struct FADT {
    struct ACPISDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;

    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t Reserved;

    uint8_t PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t PM1EventLength;
    uint8_t PM1ControlLength;
    uint8_t PM2ControlLength;
    uint8_t PMTimerLength;
    uint8_t GPE0Length;
    uint8_t GPE1Length;
    uint8_t GPE1Base;
    uint8_t CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t DutyOffset;
    uint8_t DutyWidth;
    uint8_t DayAlarm;
    uint8_t MonthAlarm;
    uint8_t Century;

    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;

    uint8_t Reserved2;
    uint32_t Flags;

    // 12 byte structure; see below for details
    struct GenericAddressStructure ResetReg;

    uint8_t ResetValue;
    uint8_t Reserved3[3];

    // 64bit pointers - Available on ACPI 2.0+
    uint64_t X_FirmwareControl;
    uint64_t X_Dsdt;

    struct GenericAddressStructure X_PM1aEventBlock;
    struct GenericAddressStructure X_PM1bEventBlock;
    struct GenericAddressStructure X_PM1aControlBlock;
    struct GenericAddressStructure X_PM1bControlBlock;
    struct GenericAddressStructure X_PM2ControlBlock;
    struct GenericAddressStructure X_PMTimerBlock;
    struct GenericAddressStructure X_GPE0Block;
    struct GenericAddressStructure X_GPE1Block;
} __attribute__((packed));

struct mpconf {          // configuration table header
    uchar signature[4];  // "PCMP"
    ushort length;       // total table length
    uchar version;       // [14]
    uchar checksum;      // all bytes must add up to 0
    uchar product[20];   // product id
    uint* oemtable;      // OEM table pointer
    ushort oemlength;    // OEM table length
    ushort entry;        // entry count
    uint* lapicaddr;     // address of local APIC
    ushort xlength;      // extended table length
    uchar xchecksum;     // extended table checksum
    uchar reserved;
};

struct mpproc {          // processor table entry
    uchar type;          // entry type (0)
    uchar apicid;        // local APIC id
    uchar version;       // local APIC verison
    uchar flags;         // CPU flags
#define MPBOOT 0x02      // This proc is the bootstrap processor.
    uchar signature[4];  // CPU signature
    uint feature;        // feature flags from CPUID instruction
    uchar reserved[8];
};

struct mpioapic {   // I/O APIC table entry
    uchar type;     // entry type (2)
    uchar apicno;   // I/O APIC id
    uchar version;  // I/O APIC version
    uchar flags;    // I/O APIC flags
    uint* addr;     // I/O APIC address
};

// Table entry types
#define MPPROC 0x00    // One per processor
#define MPBUS 0x01     // One per bus
#define MPIOAPIC 0x02  // One per I/O APIC
#define MPIOINTR 0x03  // One per bus interrupt source
#define MPLINTR 0x04   // One per system interrupt source

//PAGEBREAK!
// Blank page.
