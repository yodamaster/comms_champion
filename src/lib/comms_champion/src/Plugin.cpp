//
// Copyright 2015 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "comms_champion/Plugin.h"

namespace comms_champion
{

Plugin::Plugin() = default;
Plugin::~Plugin() = default;

bool Plugin::isApplied() const
{
    return m_ctrlInterface != nullptr;
}

void Plugin::apply(PluginControlInterface& controlInterface)
{
    assert(!isApplied());
    m_ctrlInterface = &controlInterface;
    applyImpl();
}

void Plugin::getCurrentConfig(QVariantMap& config)
{
    getCurrentConfigImpl(config);
}

QVariantMap Plugin::getCurrentConfig()
{
    QVariantMap config;
    getCurrentConfig(config);
    return config;
}

void Plugin::reconfigure(const QVariantMap& config)
{
    reconfigureImpl(config);
}

Plugin::WidgetPtr Plugin::getConfigWidget()
{
    return getConfigWidgetImpl();
}

void Plugin::getCurrentConfigImpl(QVariantMap& config)
{
    static_cast<void>(config);
}

void Plugin::reconfigureImpl(const QVariantMap& config)
{
    static_cast<void>(config);
}

Plugin::WidgetPtr Plugin::getConfigWidgetImpl()
{
    return WidgetPtr();
}

PluginControlInterface& Plugin::ctrlInterface()
{
    assert(isApplied());
    return *m_ctrlInterface;
}

}  // namespace comms_champion

