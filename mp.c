// Multiprocessor support
// Search memory for MP description structures.
// http://developer.intel.com/design/pentium/datashts/24201606.pdf

#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mp.h"
#include "x86.h"
#include "mmu.h"
#include "proc.h"

struct cpu cpus[NCPU];
int ismp;
int ncpu;
uchar ioapicid;

static uchar
sum(uchar *addr, int len)
{
  int i, sum;

  sum = 0;
  for(i=0; i<len; i++)
    sum += addr[i];
  return sum;
}

// Look for an MP structure in the len bytes at addr.
static struct mp*
mpsearch1(uint a, int len)
{
  uchar *e, *p, *addr;

  addr = P2V(a);
  e = addr+len;
  for(p = addr; p < e; p += sizeof(struct mp))
    if(memcmp(p, "_MP_", 4) == 0 && sum(p, sizeof(struct mp)) == 0)
      return (struct mp*)p;
  return 0;
}

static struct rsdp*
rsdpsearch1(uint a, int len)
{
  uchar *e, *p, *addr;

  addr = P2V(a);
  e = addr+len;
  for(p = addr; p < e; p += 16)
    if(memcmp(p, "RSD PTR ", 8) == 0 && sum(p, 20) == 0 && (((struct rsdp*)p)->revision == 0 || sum(p+20, 16) == 0))
      return (struct rsdp*)p;
  return 0;
}

// Search for the MP Floating Pointer Structure, which according to the
// spec is in one of the following three locations:
// 1) in the first KB of the EBDA;
// 2) in the last KB of system base memory;
// 3) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct mp*
mpsearch(void)
{
  uchar *bda;
  uint p;
  struct mp *mp;

  bda = (uchar *) P2V(0x400);
  if((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
    if((mp = mpsearch1(p, 1024)))
      return mp;
  } else {
    p = ((bda[0x14]<<8)|bda[0x13])*1024;
    if((mp = mpsearch1(p-1024, 1024)))
      return mp;
  }
  return mpsearch1(0xF0000, 0x10000);
}

// Search for RSDP, which according to the
// spec is in one of the following two locations:
// 1) in the first KB of the EBDA;
// 2) in the BIOS ROM between 0xE0000 and 0xFFFFF.
static struct rsdp*
rsdpsearch(void)
{
  uchar *bda;
  uint p;
  struct rsdp *rsdp;

  bda = (uchar *) P2V(0x400);
  if((p = ((bda[0x0F]<<8)| bda[0x0E]) << 4)){
    if((rsdp = rsdpsearch1(p, 1024)))
      return rsdp;
  }
  return rsdpsearch1(0xE0000, 0x20000);
}

// Search for an MP configuration table.  For now,
// don't accept the default configurations (physaddr == 0).
// Check for correct signature, calculate the checksum and,
// if correct, check the version.
// To do: check extended table checksum.
static struct mpconf*
mpconfig(struct mp **pmp)
{
  struct mpconf *conf;
  struct mp *mp;

  if((mp = mpsearch()) == 0 || mp->physaddr == 0)
    return 0;
  conf = (struct mpconf*) P2V((uint) mp->physaddr);
  if(memcmp(conf, "PCMP", 4) != 0)
    return 0;
  if(conf->version != 1 && conf->version != 4)
    return 0;
  if(sum((uchar*)conf, conf->length) != 0)
    return 0;
  *pmp = mp;
  return conf;
}

void
mpinit(void)
{
  uchar *p, *e;
  uchar *pp, *ee;
  struct mp *mp;
  struct mpconf *conf;
//  struct mpproc *proc;
  struct mpioapic *ioapic;
  uint feature;
  uint high, low;
  struct rsdp *rsdp;
  struct xsdt *xsdt;
  struct rsdt *rsdt;
  struct madt *madt;

  // x2APIC
  asm volatile("cpuid;" : "=c"(feature) : "a"(1), "c"(0));
  if (feature & (1 << 21)) {
    cprintf("x2APIC is supported!\n");
  } else {
    cprintf("x2APIC is not supported!\n");
  }
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1b));
  if (low & (1 << 11)) {
    cprintf("EN is set!\n");
  } else {
    cprintf("EN is not set!\n");
  }
  if (low & (1 << 10)) {
    cprintf("EXTD is set!\n");
  } else {
    cprintf("EXTD is not set!\n");
  }
//  low |= (1 << 10);
//  asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0x1b));

  // RSDP
  if ((rsdp = rsdpsearch())) {
    cprintf("RSDP found at %p!\n", rsdp);
    cprintf("RSDP revision is %d!\n", rsdp->revision);
    if (rsdp->revision == 2) {
      cprintf("rsdp->xsdtaddress[0]: %p\n", rsdp->xsdtaddress[0]);
      cprintf("rsdp->xsdtaddress[1]: %p\n", rsdp->xsdtaddress[1]);
      xsdt = (struct xsdt*)rsdp->xsdtaddress;
      cprintf("xsdt->header.signature: ");
      consputc(xsdt->header.signature[0]);
      consputc(xsdt->header.signature[1]);
      consputc(xsdt->header.signature[2]);
      consputc(xsdt->header.signature[3]);
      consputc('\n');
      cprintf("xsdt->header.length: %d\n", xsdt->header.length);
      cprintf("sum: %d\n", sum((uchar*)xsdt, xsdt->header.length));
      if (memcmp(xsdt->header.signature, "XSDT", 4) == 0 && sum((uchar*)xsdt, xsdt->header.length) == 0) {
        cprintf("XSDT is valid!\n");
//        for (p=(uchar*)xsdt->table, e=(uchar*)xsdt+xsdt->header.length; p<e; p+=8) {
//          madt = *(struct madt**)p;
//          if (memcmp(madt->header.signature, "APIC", 4) == 0 && sum((uchar*)madt, madt->header.length) == 0) {
//            for (pp=(uchar*)madt->table, ee=(uchar*)madt+madt->header.length; pp<ee; pp+=((struct madte*)pp)->length) {
//              cprintf("MADT: type: %d\n", ((struct madte*)pp)->type);
//            }
//            break;
//          }
//        }
//        if (p>=e) cprintf("MADT not found\n");
      } else {
        cprintf("XSDT is invalid!\n");
      }

      cprintf("rsdp->rsdtaddress: %p\n", rsdp->rsdtaddress);
      rsdt = (struct rsdt*)rsdp->rsdtaddress;
      cprintf("rsdt->header.signature: ");
      consputc(rsdt->header.signature[0]);
      consputc(rsdt->header.signature[1]);
      consputc(rsdt->header.signature[2]);
      consputc(rsdt->header.signature[3]);
      consputc('\n');
      cprintf("rsdt->header.length: %d\n", rsdt->header.length);
      cprintf("sum: %d\n", sum((uchar*)rsdt, rsdt->header.length));
      if (memcmp(rsdt->header.signature, "RSDT", 4) == 0 && sum((uchar*)rsdt, rsdt->header.length) == 0) {
        cprintf("RSDT is valid!\n");
        for (p=(uchar*)rsdt->table, e=(uchar*)rsdt+rsdt->header.length; p<e; p+=4) {
          madt = *(struct madt**)p;
          if (memcmp(madt->header.signature, "APIC", 4) == 0 && sum((uchar*)madt, madt->header.length) == 0) {
            for (pp=(uchar*)madt->table, ee=(uchar*)madt+madt->header.length; pp<ee; pp+=((struct madte*)pp)->length) {
              if (((struct madte*)pp)->type == 0) {
                cprintf("MADT: type: %d (ACPI processor ID: %d, APIC ID: %d, flags: %d)\n",
                        ((struct madte*)pp)->type,
                        ((struct madte*)pp)->acpiprocessorid,
                        ((struct madte*)pp)->apicid,
                        ((struct madte*)pp)->flags
                );
                if(ncpu < NCPU) {
                  cpus[ncpu].apicid = ((struct madte*)pp)->apicid;  // apicid may differ from ncpu
                  ncpu++;
                }
              } else {
                cprintf("MADT: type: %d\n", ((struct madte*)pp)->type);
              }
            }
            break;
          }
        }
        if (p>=e) cprintf("MADT not found\n");
      } else {
        cprintf("RSDT is invalid!\n");
      }
    }
  } else {
    cprintf("RSDP not found!\n");
  }

  if((conf = mpconfig(&mp)) == 0)
    return;
  ismp = 1;
  lapic = (uint*)conf->lapicaddr;
  for(p=(uchar*)(conf+1), e=(uchar*)conf+conf->length; p<e; ){
    switch(*p){
    case MPPROC:
//      proc = (struct mpproc*)p;
//      cprintf("MP: APIC ID: %d\n", proc->apicid);
//      if(ncpu < NCPU) {
//        cpus[ncpu].apicid = proc->apicid;  // apicid may differ from ncpu
//        ncpu++;
//      }
      p += sizeof(struct mpproc);
      continue;
    case MPIOAPIC:
      ioapic = (struct mpioapic*)p;
      ioapicid = ioapic->apicno;
      p += sizeof(struct mpioapic);
      continue;
    case MPBUS:
    case MPIOINTR:
    case MPLINTR:
      p += 8;
      continue;
    default:
      ismp = 0;
      break;
    }
  }
  if(!ismp){
    // Didn't like what we found; fall back to no MP.
    ncpu = 1;
    lapic = 0;
    ioapicid = 0;
    return;
  }

  if(mp->imcrp){
    // Bochs doesn't support IMCR, so this doesn't run on Bochs.
    // But it would on real hardware.
    outb(0x22, 0x70);   // Select IMCR
    outb(0x23, inb(0x23) | 1);  // Mask external interrupts.
  }
}
