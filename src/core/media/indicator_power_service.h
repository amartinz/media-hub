/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *      Author: Jim Hodapp <jim.hodapp@canonical.com>
 */

#include <core/dbus/dbus.h>
#include <core/dbus/fixture.h>
#include <core/dbus/object.h>
#include <core/dbus/property.h>
#include <core/dbus/service.h>
#include <core/dbus/interfaces/properties.h>
#include <core/dbus/types/stl/tuple.h>
#include <core/dbus/types/stl/vector.h>

#include <core/dbus/asio/executor.h>

#include <string>

namespace core
{

struct IndicatorPower
{
    static std::string& name()
    {
        static std::string s = "com.canonical.indicator.power.Battery";
        return s;
    }

    struct PowerLevel
    {
        static std::string name()
        {
            static std::string s = "PowerLevel";
            return s;
        }

        typedef IndicatorPower Interface;
        // Possible values: "ok", "low", "very_low", "critical"
        typedef std::string ValueType;
        static const bool readable = true;
        static const bool writable = false;
    };

}; // IndicatorPower

}
