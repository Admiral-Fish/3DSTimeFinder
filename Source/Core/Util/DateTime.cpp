/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2022 by Admiral_Fish
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

#include "DateTime.hpp"
#include <array>
#include <cstring>

consteval std::array<char[2], 100> computeNumbers()
{
    std::array<char[2], 100> strings;

    for (char i = 0; i < strings.size(); i++)
    {
        if (i < 10)
        {
            strings[i][0] = '0';
            strings[i][1] = i + '0';
        }
        else
        {
            strings[i][0] = (i / 10) + '0';
            strings[i][1] = (i % 10) + '0';
        }
    }

    return strings;
}

constexpr std::array<char[2], 100> numbers = computeNumbers();
constexpr u8 monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

inline bool isLeapYear(int year)
{
    // Since we know the years are bound between 2000 and 2099, we can simplify this check
    // return ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0));
    return (year % 4) == 0;
}

Date::Date(int year, int month, int day)
{
    int a = month < 3 ? 1 : 0;
    int y = year + 4800 - a;
    int m = month + 12 * a - 3;
    jd = day + ((153 * m + 2) / 5) - 32045 + 365 * y + (y / 4) - (y / 100) + (y / 400);
}

Date Date::addDays(int days) const
{
    return Date(jd + days);
}

int Date::daysTo(const Date &other) const
{
    return other.jd - jd;
}

int Date::year() const
{
    return getParts().year;
}

int Date::month() const
{
    return getParts().month;
}

int Date::day() const
{
    return getParts().day;
}

int Date::dayOfWeek() const
{
    return (jd + 1) % 7;
}

DateParts Date::getParts() const
{
    u32 a = jd + 32044;
    u32 b = (4 * a + 3) / 146097;
    u32 c = a - (146097 * b) / 4;

    u32 d = (4 * c + 3) / 1461;
    u32 e = c - (1461 * d) / 4;
    u32 m = (5 * e + 2) / 153;

    // int y = 100 * b + d - 4800 + (m / 10);
    // int year = y > 0 ? y : y - 1;

    u16 year = 100 * b + d - 4800 + (m / 10);
    u8 month = m + 3 - 12 * (m / 10);
    u8 day = e - ((153 * m + 2) / 5) + 1;

    return { year, month, day };
}

int Date::daysInMonth(int month, int year)
{
    if (month == 2 && isLeapYear(year))
    {
        return 29;
    }
    return monthDays[month - 1];
}

std::string Date::toString() const
{
    char buf[11] = "20  -  -  ";
    auto parts = getParts();

    std::memcpy(buf + 2, numbers[parts.year - 2000], 2);
    std::memcpy(buf + 5, numbers[parts.month], 2);
    std::memcpy(buf + 8, numbers[parts.day], 2);

    return std::string(buf, sizeof(buf) - 1);
}

int Time::addSeconds(int seconds)
{
    md += seconds;

    int days = 0;
    while (md >= 86400)
    {
        md -= 86400;
        days++;
    }
    return days;
}

int Time::hour() const
{
    return md / 3600;
}

int Time::minute() const
{
    return (md % 3600) / 60;
}

int Time::second() const
{
    return md % 60;
}

std::string Time::toString() const
{
    char buf[9] = "  :  :  ";

    std::memcpy(buf, numbers[hour()], 2);
    std::memcpy(buf + 3, numbers[minute()], 2);
    std::memcpy(buf + 6, numbers[second()], 2);

    return std::string(buf, sizeof(buf) - 1);
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second) :
    date(Date(year, month, day)), time(Time(hour, minute, second))
{
}

DateTime::DateTime(u64 milliseconds)
{
    u64 seconds = milliseconds / 1000;
    u64 days = seconds / 86400;

    seconds -= days * 86400;
    u64 hours = seconds / 3600;

    seconds -= hours * 3600;
    u64 minutes = seconds / 60;

    seconds -= minutes * 60;

    date = Date(1970, 1, 1) + Date(days);
    time = Time(hours, minutes, seconds);
}

void DateTime::addSeconds(int seconds)
{
    int days = time.addSeconds(seconds);
    date = date.addDays(days);
}

DateTime DateTime::addSecs(int seconds)
{
    DateTime dt(*this);
    dt.addSeconds(seconds);
    return dt;
}

Date DateTime::getDate() const
{
    return date;
}

Time DateTime::getTime() const
{
    return time;
}

std::string DateTime::toString() const
{
    char buf[20] = "20  -  -     :  :  ";
    auto parts = date.getParts();

    std::memcpy(buf + 2, numbers[parts.year - 2000], 2);
    std::memcpy(buf + 5, numbers[parts.month], 2);
    std::memcpy(buf + 8, numbers[parts.day], 2);
    std::memcpy(buf + 11, numbers[time.hour()], 2);
    std::memcpy(buf + 14, numbers[time.minute()], 2);
    std::memcpy(buf + 17, numbers[time.second()], 2);

    return std::string(buf, sizeof(buf) - 1);
}

u64 DateTime::toMSecsSinceEpoch() const
{
    Date epoch(1970, 1, 1);
    int days = date.jd - epoch.jd;
    return (static_cast<u64>(days) * 86400000) + (static_cast<u64>(time.md) * 1000);
}
