#pragma once

#include "core/types.h"

#include "string.h"
#include "stringview.h"

using Hash = u32;

template<typename T>
struct Hasher
{
    inline Hash operator()(T const& key);
};

constexpr Hash bytes[4] = {
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0xFF000000
};

template <>
struct Hasher<f32>
{
    inline Hash operator()(f32 const& key) const
    {
        Hash hash = *(Hash*)(&key);
        Hash shuffled = (hash >> 16) | (hash << 16);

        return shuffled ^ 0xE8BF6CDF;
    }
};

template <>
struct Hasher<s32>
{
    inline Hash operator()(s32 const& key) const
    {
        Hash hash = *(Hash*)(&key);

        // 00 00 00 FF -> 00 FF 00 00   << 16
        // 00 00 FF 00 -> 00 00 00 FF   >> 8
        // 00 FF 00 00 -> FF 00 00 00   << 8
        // FF 00 00 00 -> 00 00 FF 00   >> 16
        
        Hash shuffled = ((hash & bytes[0]) << 16) |
                        ((hash & bytes[1]) >>  8) |
                        ((hash & bytes[2]) <<  8) |
                        ((hash & bytes[3]) >> 16);

        return shuffled;
    }
};

template<>
struct Hasher<String>
{
    inline Hash operator()(String const& key) const
    {
        Hash* ptr = (Hash*) key.cstr();
        u64 count = (key.size() / 4) + 1;

        Hash hash = 0x8BDC195DF;
        while (count)
        {
            Hash val = *ptr;
            
            hash = hash + hash * val * val * count * count;
            hash = ((hash & bytes[0]) << 16) |
                   ((hash & bytes[1]) >>  8) |
                   ((hash & bytes[2]) <<  8) |
                   ((hash & bytes[3]) >> 16);

            count--;
            ptr++;
        }

        return hash;
    }
};

template<>
struct Hasher<StringView>
{
    inline Hash operator()(StringView const& key) const
    {
        Hash* ptr = (Hash*) key.cstr();
        u64 count = key.size() / 4 + 1;
        u32 rem = key.size() % 4;

        Hash hash = 0x8BDC195DF;
        while (count > 1)
        {
            Hash val = *ptr;
            
            hash = hash + hash * val * val * count * count;
            hash = ((hash & bytes[0]) << 16) |
                   ((hash & bytes[1]) >>  8) |
                   ((hash & bytes[2]) <<  8) |
                   ((hash & bytes[3]) >> 16);

            count--;
            ptr++;
        }

        {   // Remaining chars
            Hash val = *ptr;
            u32 mask = 0xFFFFFFFF >> ((4 - rem) * 8);
            val &= mask;
            
            hash = hash + hash * val * val;
            hash = ((hash & bytes[0]) << 16) |
                   ((hash & bytes[1]) >>  8) |
                   ((hash & bytes[2]) <<  8) |
                   ((hash & bytes[3]) >> 16);
        }

        return hash;
    }
};