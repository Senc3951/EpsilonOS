#pragma once

#include <kernel.hpp>

namespace kernel
{
    template <typename T>
    class Bitmap
    {
    private:
        T *m_addr;
        T m_max_value;
        u64 m_bsize, m_bitsize, m_entries;
        size_t m_entry_size;

        constexpr void get_offset(const u64 num, u64& i, u64& j) const
        {
            // Verify the number is in range
            assert(num < m_bitsize);

            i = num / m_entry_size;
            j = num % m_entry_size;
        }

        constexpr int get_bit(const u64 i, const u64 j) const
        {
            return (m_addr[i] >> j) & 1;
        }
        
        constexpr void set_bit(const u64 i, const u64 j)
        {
            m_addr[i] |= (1ULL << j);
        }
    public:
        Bitmap() : m_addr(nullptr) { }
        Bitmap(T *addr, const T max_value, const u64 bsize);

        void init(T *addr, const T max_value, const u64 bsize);

        constexpr uintptr_t addr() const { return reinterpret_cast<uintptr_t>(m_addr); }
        constexpr u64 bsize() const { return m_bsize; }
        constexpr bool is_null() const { return m_addr == nullptr; }

        s64 find_set();
        void set(const u64 num, const bool check);
        void clear(const u64 num, const bool check);
    };

    template <typename T>
    Bitmap<T>::Bitmap(T *addr, const T max_value, const u64 bsize)
    {
        init(addr, max_value, bsize);
    }
    
    template <typename T>
    void Bitmap<T>::init(T *addr, const T max_value, const u64 bsize)
    {
        m_addr = addr;
        m_max_value = max_value;
        m_bsize = bsize;
        m_bitsize = m_bsize * 8;
        m_entries = m_bsize / sizeof(T);
        m_entry_size = sizeof(T) * 8;
    }

    template <typename T>
    s64 Bitmap<T>::find_set()
    {
        for (u64 i = 0; i < m_entries; i++)
        {
            // Entire entry is used, move to the next one
            if (m_addr[i] == m_max_value)
                continue;
            
            for (u64 j = 0; j < m_entry_size; j++)
            {
                // Unused bit
                if (get_bit(i, j) == 0)
                {
                    set_bit(i, j);
                    return i * m_entry_size + j;
                }
            }
        }

        // No available space was found
        return -1;
    }

    template <typename T>
    void Bitmap<T>::set(const u64 num, const bool check)
    {
        // Get offset of number in bitmap
        uint64_t i, j;
        get_offset(num, i, j);

        // Verify the bit is unused
        assert(get_bit(i, j) == 0 || !check);

        // Mark the bit as used
        set_bit(i, j);
    }

    template <typename T>
    void Bitmap<T>::clear(const u64 num, const bool check)
    {
        // Get offset of number in bitmap
        uint64_t i, j;
        get_offset(num, i, j);

        // Verify the bit is used
        assert(get_bit(i, j) == 1 || !check);

        // Mark the bit as unused
        m_addr[i] &= ~(1ULL << j);
    }
}