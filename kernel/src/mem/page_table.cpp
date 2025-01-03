#include <mem/page_table.hpp>
#include <mem/pmm.hpp>
#include <lib/string.h>

namespace kernel::memory
{
    PageTable *PageTable::get_next_table(const size_t index, const u64 flags, const bool allocate)
    {
        PageTableEntry *entry = at(index);
        if (entry->is_present())    // Entry is valid, return pointer to next table
            return entry->get_frame().tohh().as<PageTable *>();
        
        // Entry is not present but we should not allocate a new one
        if (!allocate)
            return nullptr;
        
        // Allocate a new entry
        return create_table(entry, flags);
    }

    PageTable *PageTable::create_table(PageTableEntry *entry, const u64 flags)
    {
        // Allocate a new page table
        Address table_address = PhysicalMemoryManager::instance().allocate_frame();
        if (table_address.is_null() || !table_address.is_page_aligned())
            panic("failed allocating page table for entry at %p", entry);

        // Reset the table
        PageTable *page_table = table_address.tohh().as<PageTable *>();
        page_table->reset();
        
        // Map the table to the table entry
        entry->set(table_address, flags);
        return page_table;
    }
    
    void PageTable::reset()
    {
        memset(m_entries, 0, sizeof(m_entries));
    }
}