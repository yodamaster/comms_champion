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

#include <cstdint>
#include <cassert>
#include <memory>

#include "comms/field/Bitfield.h"
#include "NumericValueWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class BitfieldWrapper : public NumericValueWrapper<long long unsigned>
{
    using Base = NumericValueWrapper<long long unsigned>;
public:
    using Base::NumericValueWrapper;
};

template <typename TField>
class BitfieldWrapperT : public NumericValueWrapperT<BitfieldWrapper, TField>
{
    using Base = NumericValueWrapperT<BitfieldWrapper, TField>;
    using Field = TField;
    static_assert(comms::field::isBitfield<Field>(), "Must be of Bitfield field type");

    using ValueType = typename Field::ValueType;
    using UnderlyingType = typename Base::UnderlyingType;
    static_assert(sizeof(ValueType) <= sizeof(UnderlyingType), "This wrapper cannot handle provided field.");
    static_assert(std::is_signed<ValueType>::value || (sizeof(ValueType) < sizeof(UnderlyingType)),
        "This wrapper cannot handle provided field.");

public:
    BitfieldWrapperT(Field& field)
      : Base(field)
    {
    }

    BitfieldWrapperT(const BitfieldWrapperT&) = default;
    BitfieldWrapperT(BitfieldWrapperT&&) = default;
    virtual ~BitfieldWrapperT() = default;

    BitfieldWrapperT& operator=(const BitfieldWrapperT&) = delete;

};

using BitfieldWrapperPtr = std::unique_ptr<BitfieldWrapper>;

template <typename TField>
BitfieldWrapperPtr
makeBitfieldWrapper(TField& field)
{
    return
        BitfieldWrapperPtr(
            new BitfieldWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion