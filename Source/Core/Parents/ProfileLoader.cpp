/*
 * This file is part of 3DSTimeFinder
 * Copyright (C) 2019-2021 by Admiral_Fish
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

#include "ProfileLoader.hpp"
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

using json = nlohmann::json;

namespace
{
    std::string path;

    json readJson()
    {
        json j;

        std::ifstream read(path);
        if (read.is_open())
        {
            j = json::parse(read, nullptr, false);
        }

        return j.is_discarded() ? json() : j;
    }

    void writeJson(const json &j)
    {
        std::ofstream write(path);
        write << j.dump();
        write.close();
    }
}

namespace ProfileLoader
{
    bool init(const std::string &location)
    {
        path = location;

        bool exists = std::filesystem::exists(path);
        if (!exists)
        {
            std::ofstream json(path);
            json << "{}";
            json.close();
        }

        return exists;
    }
}

namespace ProfileLoader6
{
    namespace
    {
        Profile6 getProfile(const json &j)
        {
            std::string name = j["name"].get<std::string>();
            u32 saveVariable = std::stoul(j["save"].get<std::string>(), nullptr, 16);
            u32 timeVariable = std::stoul(j["time"].get<std::string>(), nullptr, 16);
            Game version = j["version"].get<Game>();
            u16 tid = j["tid"].get<u16>();
            u16 sid = j["sid"].get<u16>();
            bool shinyCharm = j["charm"].get<bool>();
            return Profile6(name, saveVariable, timeVariable, tid, sid, version, shinyCharm);
        }

        json getJson(const Profile6 &profile)
        {
            json j;
            j["name"] = profile.getName();
            std::stringstream stream1;
            stream1 << std::hex << profile.getSaveVariable();
            j["save"] = stream1.str();
            std::stringstream stream2;
            stream2 << std::hex << profile.getTimeVariable();
            j["time"] = stream2.str();
            j["version"] = profile.getVersion();
            j["tid"] = profile.getTID();
            j["sid"] = profile.getSID();
            j["charm"] = profile.getShinyCharm();
            return j;
        }
    }

    std::vector<Profile6> getProfiles()
    {
        std::vector<Profile6> profiles;

        json j = readJson();
        const auto &gen6 = j["gen6"];
        std::transform(gen6.begin(), gen6.end(), std::back_inserter(profiles), [](const json &j) { return getProfile(j); });

        return profiles;
    }

    void addProfile(const Profile6 &profile)
    {
        json j = readJson();

        auto &gen6 = j["gen6"];
        gen6.emplace_back(getJson(profile));

        writeJson(j);
    }

    void removeProfile(const Profile6 &remove)
    {
        json j = readJson();

        auto &gen6 = j["gen6"];
        for (size_t i = 0; i < gen6.size(); i++)
        {
            Profile6 profile = getProfile(gen6[i]);

            if (profile == remove)
            {
                gen6.erase(gen6.begin() + i);

                writeJson(j);
                break;
            }
        }
    }

    void updateProfile(const Profile6 &update, const Profile6 &original)
    {
        json j = readJson();

        auto &gen6 = j["gen6"];
        for (auto &i : gen6)
        {
            Profile6 profile = getProfile(i);

            if (original == profile && original != update)
            {
                i = getJson(update);

                writeJson(j);
                break;
            }
        }
    }
}

namespace ProfileLoader7
{
    namespace
    {
        Profile7 getProfile(const json &j)
        {
            std::string name = j["name"].get<std::string>();
            u32 offset = j["offset"].get<u32>();
            u32 tick = std::stoul(j["tick"].get<std::string>(), nullptr, 16);
            u16 tid = j["tid"].get<u16>();
            u16 sid = j["sid"].get<u16>();
            Game version = j["version"].get<Game>();
            bool shinyCharm = j["charm"].get<bool>();
            return Profile7(name, offset, tick, tid, sid, version, shinyCharm);
        }

        json getJson(const Profile7 &profile)
        {
            json j;
            j["name"] = profile.getName();
            j["offset"] = profile.getOffset();
            std::stringstream stream;
            stream << std::hex << profile.getTick();
            j["tick"] = stream.str();
            j["version"] = profile.getVersion();
            j["tid"] = profile.getTID();
            j["sid"] = profile.getSID();
            j["charm"] = profile.getShinyCharm();
            return j;
        }
    }

    std::vector<Profile7> getProfiles()
    {
        std::vector<Profile7> profiles;

        json j = readJson();
        const auto &gen7 = j["gen7"];
        std::transform(gen7.begin(), gen7.end(), std::back_inserter(profiles), [](const json &j) { return getProfile(j); });

        return profiles;
    }

    void addProfile(const Profile7 &profile)
    {
        json j = readJson();

        auto &gen7 = j["gen7"];
        gen7.emplace_back(getJson(profile));

        writeJson(j);
    }

    void removeProfile(const Profile7 &remove)
    {
        json j = readJson();

        auto &gen7 = j["gen7"];
        for (size_t i = 0; i < gen7.size(); i++)
        {
            Profile7 profile = getProfile(gen7[i]);

            if (profile == remove)
            {
                gen7.erase(gen7.begin() + i);

                writeJson(j);
                break;
            }
        }
    }

    void updateProfile(const Profile7 &update, const Profile7 &original)
    {
        json j = readJson();

        auto &gen7 = j["gen7"];
        for (auto &i : gen7)
        {
            Profile7 profile = getProfile(i);

            if (original == profile && original != update)
            {
                i = getJson(update);

                writeJson(j);
                break;
            }
        }
    }
}
