//
// Copyright 2014 - 2016 (C). Alex Robenko. All rights reserved.
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


#pragma once

#include <type_traits>
#include <cassert>
#include <iterator>
#include <algorithm>
#include <vector>

#include "comms/comms.h"

#include "Message.h"
#include "MessageHandler.h"

namespace comms_champion
{

template <template<typename...> class TMessageBase, typename... TOptions>
class MessageBase :
        public comms_champion::Message,
        public TMessageBase<
            comms::option::IdInfoInterface,
            comms::option::ReadIterator<const std::uint8_t*>,
            comms::option::WriteIterator<std::back_insert_iterator<std::vector<std::uint8_t> > >,
            comms::option::Handler<MessageHandler>,
            comms::option::ValidCheckInterface,
            comms::option::LengthInfoInterface,
            comms::option::RefreshInterface,
            TOptions...>
{
    using CCBase = comms_champion::Message;
    using CommsBase =
        TMessageBase<
            comms::option::IdInfoInterface,
            comms::option::ReadIterator<const std::uint8_t*>,
            comms::option::WriteIterator<std::back_insert_iterator<std::vector<std::uint8_t> > >,
            comms::option::Handler<MessageHandler>,
            comms::option::ValidCheckInterface,
            comms::option::LengthInfoInterface,
            comms::option::RefreshInterface,
            TOptions...>;
public:
    typedef typename CommsBase::Handler Handler;

    MessageBase() = default;
    MessageBase(const MessageBase&) = default;
    MessageBase(MessageBase&&) = default;
    ~MessageBase() = default;
    MessageBase& operator=(const MessageBase& other)
    {
        CommsBase::operator=(other);
        return *this;
    }

    MessageBase& operator=(MessageBase&& other)
    {
        CommsBase::operator=(std::move(other));
        return *this;
    }
protected:

    virtual bool refreshMsgImpl() override
    {
        return CommsBase::refresh();
    }

    virtual bool isValidImpl() const override
    {
        return CommsBase::valid();
    }

    virtual DataSeq encodeDataImpl() const override
    {
        typedef typename CommsBase::WriteIterator WriteIterator;
        typedef typename std::iterator_traits<WriteIterator>::iterator_category Tag;

        static_assert(
            std::is_base_of<std::random_access_iterator_tag, Tag>::value ||
            std::is_base_of<std::output_iterator_tag, Tag>::value,
            "Only random access or output iterator is supported for data encoding.");

        return encodeDataIntenal(Tag());
    }

    virtual bool decodeDataImpl(const DataSeq& data) override
    {
        typedef typename CommsBase::ReadIterator ReadIterator;
        typedef typename std::iterator_traits<ReadIterator>::iterator_category Tag;
        static_assert(
            std::is_base_of<std::random_access_iterator_tag, Tag>::value,
            "Only random access iterator is supported for data decoding.");

        return decodeDataRandomAccess(data);
    }

private:
    struct UseBackInserterTag {};
    struct WriteThanCopyTag {};
    struct UseDataSeqIterTag {};
    struct UsePointerTag {};
    struct OtherInputIterTag {};

    DataSeq encodeDataIntenal(std::random_access_iterator_tag) const
    {
        return encodeDataRandomAccess();
    }

    DataSeq encodeDataIntenal(std::output_iterator_tag) const
    {
        typedef typename CommsBase::WriteIterator WriteIterator;
        typedef typename
            std::conditional<
                std::is_same<std::back_insert_iterator<DataSeq>, WriteIterator>::value,
                UseBackInserterTag,
                WriteThanCopyTag
            >::type Tag;

        static_assert(
            std::is_same<Tag, UseBackInserterTag>::value,
            "Currently only back_insert_iterator is supported.");
        return encodeDataWithOutputIter(Tag());
    }

    DataSeq encodeDataRandomAccess() const
    {
        DataSeq data;
        try {
            do {
                data.resize(CommsBase::length());
                typename CommsBase::ReadIterator iter = &data[0];
                auto es = CommsBase::write(iter, data.size());
                if (es != comms::ErrorStatus::Success) {
                    assert(!"Data serialisation failed");
                    data.clear();
                    break;
                }

                typename CommsBase::ReadIterator begIter = &data[0];
                data.resize(
                    static_cast<std::size_t>(std::distance(begIter, iter)));
            } while (false);
        }
        catch (...) {
            data.clear();
        }
        return data;
    }

    DataSeq encodeDataWithOutputIter(UseBackInserterTag) const
    {
        DataSeq data;
        auto iter = std::back_inserter(data);
        auto es = CommsBase::write(iter, data.max_size());
        assert(es == comms::ErrorStatus::Success);
        static_cast<void>(es);
        return data;
    }

    bool decodeDataRandomAccess(const DataSeq& data)
    {
        typedef typename CommsBase::ReadIterator ReadIterator;
        typedef typename
            std::conditional<
                std::is_same<DataSeq::const_iterator, ReadIterator>::value,
                UseDataSeqIterTag,
                typename std::conditional<
                    std::is_pointer<ReadIterator>::value,
                    UsePointerTag,
                    OtherInputIterTag
                >::type
            >::type Tag;

        static_assert(
            !std::is_same<Tag, OtherInputIterTag>::value,
            "Unexpected read iterator");

        return decodeDataRandomAccessInternal(data, Tag());
    }

    bool decodeDataRandomAccessInternal(const DataSeq& data, UseDataSeqIterTag)
    {
        auto iter = data.begin();
        return decodeDataRandomAccessWithIter(iter, data.size());
    }

    bool decodeDataRandomAccessInternal(const DataSeq& data, UsePointerTag)
    {
        typedef typename CommsBase::ReadIterator ReadIterator;
        ReadIterator iter = &data[0];
        return decodeDataRandomAccessWithIter(iter, data.size());
    }

    template <typename TIter>
    bool decodeDataRandomAccessWithIter(TIter& iter, std::size_t bufSize)
    {
        auto es = CommsBase::read(iter, bufSize);
        return es == comms::ErrorStatus::Success;
    }
};

}  // namespace comms_champion


