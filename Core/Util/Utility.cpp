/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019 by Admiral_Fish
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

#include "Utility.hpp"
#include <Core/RNG/SHA256.hpp>

static const QStringList natures = { "Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed",
    "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest", "Mild", "Quiet", "Bashful", "Rash",
    "Calm", "Gentle", "Sassy", "Careful", "Quirky" };

static const QStringList hiddenPowers = { "Fighting", "Flying", "Poison", "Ground", "Rock", "Bug", "Ghost", "Steel",
    "Fire", "Water", "Grass", "Electric", "Psychic", "Ice", "Dragon", "Dark" };

static const QStringList genderRatios
    = { "Genderless", "♂1 : ♀1", "♂7 : ♀1", "♂3 : ♀1", "♂1 : ♀3", "♂1 : ♀7", "♂ Only", "♀ Only" };

u64 Utility::getCitraTime(const QDateTime &dateTime, u64 offset)
{
    return static_cast<u64>(dateTime.toMSecsSinceEpoch()) + offset - 946684800000;
}

u64 Utility::getNormalTime(u64 time, u64 offset)
{
    return time + 946684800000 - offset;
}

u32 Utility::calcInitialSeed(u32 tick, u64 epoch)
{
    return SHA256::hash(tick, static_cast<u32>(epoch & 0xffffffff), static_cast<u32>(epoch >> 32));
}

QString Utility::getNature(u8 nature)
{
    return natures.at(nature);
}

QString Utility::getHiddenPower(u8 hiddenPower)
{
    return hiddenPowers.at(hiddenPower);
}

QStringList Utility::getNatures()
{
    return natures;
}

QStringList Utility::getHiddenPowers()
{
    return hiddenPowers;
}

QStringList Utility::getGenderRatios()
{
    return genderRatios;
}
