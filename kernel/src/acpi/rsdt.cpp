#include <acpi/rsdt.hpp>
#include <mem/address.hpp>
#include <lib/string.h>
#include <log.hpp>

namespace kernel::acpi
{
    constexpr size_t TABLE_NAME_SIZE = 4;

    struct Rsdp
    {
        char signature[8];
        uint8_t checksum;
        char oemid[6];
        uint8_t revision;
        uint32_t rsdt_address;
    } __packed__;

    struct Xsdp
    {
        Rsdp rsdp;
        uint32_t length;
        uint64_t xsdt_address;
        uint8_t extended_checksum;
        uint8_t reserved[3];
    } __packed__;

    struct Xsdt
    {
        RsdtHeader header;
        uint64_t tables[]; 
    } __packed__;

    RSDT RSDT::m_instance;

    void RSDT::init()
    {
        Address rsdp_address(rsdp_request.response->address);
        Rsdp *rsdp = rsdp_address.as<Rsdp *>();
        m_v1 = rsdp->revision == 0;
        
        // Validate rsdp
        assert(validate_table(rsdp, sizeof(Rsdp)));
        if (!m_v1)  // New version
            assert(validate_table(&(((Xsdp *)rsdp)->length), sizeof(Xsdp) - sizeof(Rsdp)));
        
        dmesgln("RSDPv%d at %p", m_v1 ? 1 : 2, rsdp);

        Address rsdt_addr(m_v1 ? (uintptr_t)rsdp->rsdt_address : ((Xsdp *)rsdp)->xsdt_address);
        rsdt_init(rsdt_addr.tohh().as<Rsdt *>());
    }

    RsdtHeader *RSDT::find_table(const char *name)
    {
        for (size_t i = 0; i < m_table_count; i++)
        {
            Address table_header_addr;
            if (m_v1)
                table_header_addr.set(m_rsdt->tables[i]);
            else
                table_header_addr.set(((Xsdt *)m_rsdt)->tables[i]);
            
            // Validate table
            RsdtHeader *table_header = table_header_addr.tohh().as<RsdtHeader *>();
            if (!memcmp(table_header->signature, name, TABLE_NAME_SIZE))
                return table_header;
        }
        
        return nullptr;
    }

    void RSDT::rsdt_init(Rsdt *rsdt)
    {
        // Validate rsdt
        m_rsdt = rsdt;
        assert(validate_table(reinterpret_cast<void *>(rsdt), rsdt->header.length));
        
        // Get table count
        RsdtHeader *header = &rsdt->header;
        m_table_count = (header->length - sizeof(RsdtHeader)) / 4;
        if (!m_v1)
            m_table_count /= 2;
        
        // Iterate tables
        dmesgln("RSDTv%d at %p with %u tables: ", m_v1 ? 1 : 2, rsdt, m_table_count);
        for (size_t i = 0; i < m_table_count; i++)
        {
            Address table_header_addr;
            if (m_v1)
                table_header_addr.set(rsdt->tables[i]);
            else
                table_header_addr.set(((Xsdt *)rsdt)->tables[i]);
            
            // Validate table
            RsdtHeader *table_header = table_header_addr.tohh().as<RsdtHeader *>();
            assert(validate_table(table_header, table_header->length));
                        
            // Print found table            
            dmesgln("`%.4s`", table_header->signature);
        }
    }

    bool RSDT::validate_table(void *ptr, size_t count)
    {
        u8 sum = 0;
        char *p = (char *)ptr;
        for (size_t i = 0; i < count; i++)
            sum += p[i];
        
        return sum == 0;
    }
}