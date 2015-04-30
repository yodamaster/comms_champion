//
// Copyright 2014 (C). Alex Robenko. All rights reserved.
//

// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.


#pragma once

#include "comms/options.h"
#include "comms/util/SizeToType.h"

namespace comms
{

namespace field
{

namespace details
{

template <typename TField, typename... TOptions>
class BitmaskValueBase;

template <typename TField>
class BitmaskValueBase<TField> : public TField
{
protected:
    typedef long long unsigned ValueType;

    static const std::size_t SerialisedLen = sizeof(ValueType);
    static const bool BitZeroIsMsb = false;
    static const bool HasCustomInitialiser = false;
    static const bool HasCustomValidator = false;
};


template <typename TField, std::size_t TLen, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::FixedLength<TLen>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{

    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::FixedLength<TLen> Option;

protected:
    using Base::BitmaskValueBase;

    static const std::size_t SerialisedLen = Option::Value;
    using ValueType = typename comms::util::SizeToType<SerialisedLen, false>::Type;
};

template <typename TField, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::BitIndexingStartsFromMsb,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;

protected:
    using Base::BitmaskValueBase;

    static const bool BitZeroIsMsb = true;
};

template <typename TField, typename T, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::DefaultValueInitialiser<T>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::DefaultValueInitialiser<T> Option;

protected:
    using Base::BitmaskValueBase;

    typedef typename Option::Type DefaultValueInitialiser;
    static const bool HasCustomInitialiser = true;
};


template <typename TField, typename T, typename... TOptions>
class BitmaskValueBase<
    TField,
    comms::option::ContentsValidator<T>,
    TOptions...> : public BitmaskValueBase<TField, TOptions...>
{
    typedef BitmaskValueBase<TField, TOptions...> Base;
    typedef comms::option::ContentsValidator<T> Option;

protected:
    using Base::BitmaskValueBase;

    typedef typename Option::Type ContentsValidator;
    static const bool HasCustomValidator = true;
};


}  // namespace details

}  // namespace field

}  // namespace comms

