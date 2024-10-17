#include <mem/physical_address.hpp>
#include <mem/pmm.hpp>

namespace kernel::memory
{
    PhysicalAddress::PhysicalAddress(const uintptr_t addr) : Address(addr, FRAME_SIZE) { }
}