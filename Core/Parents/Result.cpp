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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110255301, USA.
 */

#include "Result.hpp"

Result::Result(u32 seed, u16 tid, u16 sid) : seed(seed), tsv((tid ^ sid) >> 4), ivs { 255, 255, 255, 255, 255, 255 }
{
}

QString Result::getDateTime() const
{
    return target.toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));
}

void Result::setTarget(const QDateTime &target)
{
    this->target = target;
}

u32 Result::getSeed() const
{
    return seed;
}

u32 Result::getPID() const
{
    return pid;
}

void Result::setPID(const u32 &pid)
{
    this->pid = pid;
    psv = ((pid >> 16) ^ (pid & 0xffff)) >> 4;
    shiny = psv == tsv;
}

u32 Result::getEC() const
{
    return ec;
}

void Result::setEC(const u32 &ec)
{
    this->ec = ec;
}

u32 Result::getFrame() const
{
    return frame;
}

void Result::setFrame(const u32 &frame)
{
    this->frame = frame;
}

u16 Result::getPSV() const
{
    return psv;
}

u8 Result::getHiddenPower() const
{
    return hiddenPower;
}

void Result::calcHiddenPower()
{
    hiddenPower = ((((ivs.at(0) & 1) + 2 * (ivs.at(1) & 1) + 4 * (ivs.at(2) & 1) + 8 * (ivs.at(5) & 1)
                        + 16 * (ivs.at(3) & 1) + 32 * (ivs.at(4) & 1))
                       * 15)
        / 63);
}

QString Result::getAbilityString() const
{
    if (ability == 2)
    {
        return "H";
    }

    return QString::number(ability + 1);
}

u8 Result::getAbility() const
{
    return ability;
}

void Result::setAbility(const u8 &ability)
{
    this->ability = ability;
}

u8 Result::getNature() const
{
    return nature;
}

void Result::setNature(const u8 &nature)
{
    this->nature = nature;
}

QString Result::getGenderString() const
{
    return gender == 0 ? "♂" : gender == 1 ? "♀" : "-";
}

u8 Result::getGender() const
{
    return gender;
}

void Result::setGender(const u8 &gender)
{
    this->gender = gender;
}

u8 Result::getIV(u8 index) const
{
    return ivs[index];
}

void Result::setIV(const u8 &index, const u8 &iv)
{
    ivs[index] = iv;
}

void Result::setIVs(const std::array<u8, 6> &ivs)
{
    this->ivs = ivs;
}

bool Result::getShiny() const
{
    return shiny;
}

void Result::setShiny(const bool &shiny)
{
    this->shiny = shiny;
}
