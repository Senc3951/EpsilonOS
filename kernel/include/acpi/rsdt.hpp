#pragma once

#include <kernel.hpp>

namespace kernel::acpi
{
    struct RsdtHeader
    {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oemid[6];
        char oem_table_id[8];
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
    } __packed__;

    class RSDT
    {
    private:
        struct Rsdt
        {
            RsdtHeader header;
            uint32_t tables[]; 
        } __packed__;
        
        static RSDT m_instance;
        
        Rsdt *m_rsdt;
        size_t m_table_count;
        bool m_v1;

        bool validate_table(void *ptr, size_t count);
        void rsdt_init(Rsdt *rsdt);

        RSDT() { }
    public:
        static constexpr RSDT& instance() { return m_instance; }
        
        void init();
        RsdtHeader *find_table(const char *name);
        
        // Delete the copy constructor & assignment operator
        RSDT(const RSDT&) = delete;
        RSDT& operator=(const RSDT&) = delete;
    };
}