/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2020 by Admiral_Fish
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef RNGLIST_HPP
#define RNGLIST_HPP

#include <Core/Global.hpp>

template <typename IntegerType, typename RNGType, u32 size>
class RNGList
{
public:
    explicit RNGList(RNGType &rng) : rng(rng), head(0), tail(size - 1), pointer(0)
    {
        for (u32 i = 0; i < size; i++)
        {
            list[i] = this->rng.next();
        }
    }

    RNGList(const RNGList &) = delete;

    void operator=(const RNGList &) = delete;

    void advanceStates(u32 frames)
    {
        for (u32 frame = 0; frame < frames; frame++)
        {
            advanceState();
        }
    }

    void advanceState()
    {
        if (++tail == size)
        {
            tail = 0;
        }

        list[head++] = rng.next();
        if (head == size)
        {
            head = 0;
        }

        pointer = head;
    }

    void advanceFrames(u32 frames)
    {
        pointer += frames;
        if (pointer >= size)
        {
            pointer -= size;
        }
    }

    IntegerType getValue()
    {
        if (pointer == size)
        {
            pointer = 0;
        }

        return list[pointer++];
    }

    void resetState()
    {
        pointer = head;
    }

private:
    RNGType rng;
    IntegerType list[size];
    u32 head, tail, pointer;
};

#endif // RNGLIST_HPP
