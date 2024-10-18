#include <mem/address.hpp>
#include <mem/pmm.hpp>
#include <mem/page_table.hpp>

namespace kernel::memory
{
    bool PhysicalAddress::is_frame_aligned() const
    {
        return ((m_addr & (FRAME_SIZE - 1)) == 0);
    }
    
    bool VirtualAddress::is_page_aligned() const
    {
        return ((m_addr & (PAGE_SIZE - 1)) == 0);
    }
}