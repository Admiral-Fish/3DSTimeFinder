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

#ifndef RESULT_HPP
#define RESULT_HPP

#include <Core/Global.hpp>
#include <QDateTime>
#include <QVector>

class Result
{
public:
    QString getDateTime() const;
    void setTarget(const QDateTime &target);
    u32 getSeed() const;
    u32 getPID() const;
    void setPID(const u32 &pid);
    u32 getEC() const;
    void setEC(const u32 &ec);
    u32 getFrame() const;
    void setFrame(const u32 &frame);
    u16 getPSV() const;
    u8 getHiddenPower() const;
    void calcHiddenPower();
    QString getAbilityString() const;
    u8 getAbility() const;
    void setAbility(const u8 &ability);
    u8 getNature() const;
    void setNature(const u8 &nature);
    QString getGenderString() const;
    u8 getGender() const;
    void setGender(const u8 &gender);
    u8 getIV(u8 index) const;
    void setIV(const u8 &index, const u8 &iv);
    void setIVs(const QVector<u8> &ivs);
    bool getShiny() const;
    void setShiny(const bool &shiny);

protected:
    QDateTime target;
    u32 seed {}, pid {}, ec {}, frame {};
    u16 psv {}, tsv {};
    u8 hiddenPower {}, ability {}, nature {}, gender {};
    QVector<u8> ivs;
    bool shiny {};
};

#endif // RESULT_HPP
