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

#include "comms/comms.h"

#include "FieldWrapper.h"

namespace comms_champion
{

namespace field_wrapper
{

class UnknownValueWrapper : public FieldWrapper
{
public:
    virtual ~UnknownValueWrapper() {}
};

template <typename TField>
class UnknownValueWrapperT : public FieldWrapperT<UnknownValueWrapper, TField>
{
    using Base = FieldWrapperT<UnknownValueWrapper, TField>;
    using Field = TField;
    using SerialisedSeq = typename Base::SerialisedSeq;

public:
    UnknownValueWrapperT(Field& field)
      : Base(field)
    {
    }

    UnknownValueWrapperT(const UnknownValueWrapperT&) = default;
    UnknownValueWrapperT(UnknownValueWrapperT&&) = default;
    virtual ~UnknownValueWrapperT() = default;

    UnknownValueWrapperT& operator=(const UnknownValueWrapperT&) = delete;

protected:

    virtual SerialisedSeq getSerialisedValueImpl() const override
    {
        auto& field = Base::field();
        SerialisedSeq value;
        value.reserve(field.length());
        auto iter = std::back_inserter(value);
        field.write(iter, value.max_size());
        return value;
    }

    virtual bool setSerialisedValueImpl(const SerialisedSeq& value) override
    {
        if (value.empty()) {
            return false;
        }

        const std::uint8_t* iter = &value[0];
        auto es = Base::field().read(iter, value.size());
        return es == comms::ErrorStatus::Success;
    }

};

using UnknownValueWrapperPtr = std::unique_ptr<UnknownValueWrapper>;

template <typename TField>
UnknownValueWrapperPtr
makeUnknownValueWrapper(TField& field)
{
    return
        UnknownValueWrapperPtr(
            new UnknownValueWrapperT<TField>(field));
}

}  // namespace field_wrapper

}  // namespace comms_champion