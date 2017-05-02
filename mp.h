// See MultiProcessor Specification Version 1.[14]

struct mp {             // floating pointer
  uchar signature[4];           // "_MP_"
  void *physaddr;               // phys addr of MP config table
  uchar length;                 // 1
  uchar specrev;                // [14]
  uchar checksum;               // all bytes must add up to 0
  uchar type;                   // MP system config type
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

struct mpconf {         // configuration table header
  uchar signature[4];           // "PCMP"
  ushort length;                // total table length
  uchar version;                // [14]
  uchar checksum;               // all bytes must add up to 0
  uchar product[20];            // product id
  uint *oemtable;               // OEM table pointer
  ushort oemlength;             // OEM table length
  ushort entry;                 // entry count
  uint *lapicaddr;              // address of local APIC
  ushort xlength;               // extended table length
  uchar xchecksum;              // extended table checksum
  uchar reserved;
};

struct mpproc {         // processor table entry
  uchar type;                   // entry type (0)
  uchar apicid;                 // local APIC id
  uchar version;                // local APIC verison
  uchar flags;                  // CPU flags
    #define MPBOOT 0x02           // This proc is the bootstrap processor.
  uchar signature[4];           // CPU signature
  uint feature;                 // feature flags from CPUID instruction
  uchar reserved[8];
};

struct mpioapic {       // I/O APIC table entry
  uchar type;                   // entry type (2)
  uchar apicno;                 // I/O APIC id
  uchar version;                // I/O APIC version
  uchar flags;                  // I/O APIC flags
  uint *addr;                  // I/O APIC address
};

// Table entry types
#define MPPROC    0x00  // One per processor
#define MPBUS     0x01  // One per bus
#define MPIOAPIC  0x02  // One per I/O APIC
#define MPIOINTR  0x03  // One per bus interrupt source
#define MPLINTR   0x04  // One per system interrupt source

//PAGEBREAK!
// Blank page.
