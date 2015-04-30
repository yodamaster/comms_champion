//
// Copyright 2013 (C). Alex Robenko. All rights reserved.
//

// This library is free software: you can redistribute it and/or modify
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

#include <tuple>
#include <type_traits>

#include "AlignedUnion.h"

namespace comms
{

namespace util
{

template <typename TType>
struct IsTuple
{
    /// @brief By default Value has value false. Will be true for any
    /// variant of std::tuple.
    static const bool Value = false;
};

template <typename... TArgs>
struct IsTuple<std::tuple<TArgs...> >
{
    static const bool Value = true;
};

//----------------------------------------

template <typename TType, typename TTuple>
class IsInTuple
{
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");
public:
    static const bool Value = false;
};

template <typename TType, typename TFirst, typename... TRest>
class IsInTuple<TType, std::tuple<TFirst, TRest...> >
{
public:
    static const bool Value =
        std::is_same<TType, TFirst>::value ||
        IsInTuple<TType, std::tuple<TRest...> >::Value;
};

template <typename TType>
class IsInTuple<TType, std::tuple<> >
{
public:
    static const bool Value = false;
};

//----------------------------------------

template <typename TTuple>
struct TupleAsAlignedUnion
{
    /// @cond DOCUMENT_STATIC_ASSERT
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");
    /// @endcond

    /// @brief Type definition is invalid for any type that is not std::tuple.
    typedef void Type;
};

template <typename... TTypes>
struct TupleAsAlignedUnion<std::tuple<TTypes...> >
{
    typedef typename AlignedUnion<TTypes...>::Type Type;
};

//----------------------------------------

template <typename TTuple>
struct TupleIsUnique
{
    static_assert(IsTuple<TTuple>::Value, "TTuple must be std::tuple");

    static const bool Value = false;
};

template <typename TFirst, typename... TRest>
struct TupleIsUnique<std::tuple<TFirst, TRest...> >
{
    static const bool Value =
        (!IsInTuple<TFirst, std::tuple<TRest...> >::Value) &&
        TupleIsUnique<std::tuple<TRest...> >::Value;
};

template <>
struct TupleIsUnique<std::tuple<> >
{
    static const bool Value = true;
};

//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleForEachHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        func(std::get<Idx>(std::forward<TTuple>(tuple)));
        TupleForEachHelper<TRem - 1>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

template <typename TTuple, typename TFunc>
void tupleForEach(TTuple&& tuple, TFunc&& func)
{
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachHelper<TupleSize>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}
//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleForEachWithIdxHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        func(std::get<Idx>(std::forward<TTuple>(tuple)), Idx);
        TupleForEachWithIdxHelper<TRem - 1>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachWithIdxHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

template <typename TTuple, typename TFunc>
void tupleForEachWithIdx(TTuple&& tuple, TFunc&& func)
{
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachWithIdxHelper<TupleSize>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}

namespace details
{

template <std::size_t TRem>
class TupleForEachWithTemplateParamIdxHelper
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        func.template operator()<Idx>(std::get<Idx>(std::forward<TTuple>(tuple)));
        TupleForEachWithTemplateParamIdxHelper<TRem - 1>::exec(
            std::forward<TTuple>(tuple),
            std::forward<TFunc>(func));
    }
};

template <>
class TupleForEachWithTemplateParamIdxHelper<0>
{

public:
    template <typename TTuple, typename TFunc>
    static void exec(TTuple&& tuple, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
    }
};

}  // namespace details

template <typename TTuple, typename TFunc>
void tupleForEachWithTemplateParamIdx(TTuple&& tuple, TFunc&& func)
{
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    details::TupleForEachWithTemplateParamIdxHelper<TupleSize>::exec(
        std::forward<TTuple>(tuple),
        std::forward<TFunc>(func));
}


//----------------------------------------

namespace details
{

template <std::size_t TRem>
class TupleAccumulateHelper
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
    static TValue exec(TTuple&& tuple, const TValue& value, TFunc&& func)
    {
        typedef typename std::decay<TTuple>::type Tuple;
        static_assert(IsTuple<Tuple>::Value, "TTuple must be std::tuple");
        static const std::size_t TupleSize = std::tuple_size<Tuple>::value;
        static_assert(TRem <= TupleSize, "Incorrect TRem");

        static const std::size_t Idx = TupleSize - TRem;
        return TupleAccumulateHelper<TRem - 1>::exec(
                    std::forward<TTuple>(tuple),
                    func(value, std::get<Idx>(std::forward<TTuple>(tuple))),
                    std::forward<TFunc>(func));
    }
};

template <>
class TupleAccumulateHelper<0>
{

public:
    template <typename TTuple, typename TValue, typename TFunc>
    static TValue exec(TTuple&& tuple, const TValue& value, TFunc&& func)
    {
        static_cast<void>(tuple);
        static_cast<void>(func);
        return value;
    }
};

}  // namespace details

template <typename TTuple, typename TValue, typename TFunc>
TValue tupleAccumulate(TTuple&& tuple, const TValue& value, TFunc&& func)
{
    typedef typename std::decay<TTuple>::type Tuple;
    static const std::size_t TupleSize = std::tuple_size<Tuple>::value;

    return details::TupleAccumulateHelper<TupleSize>::exec(
                std::forward<TTuple>(tuple),
                value,
                std::forward<TFunc>(func));
}

//----------------------------------------

template <typename TFirst, typename TSecond>
struct TupleCat
{
    static_assert(IsTuple<TFirst>::Value, "TFirst must be tuple");
    static_assert(IsTuple<TSecond>::Value, "TSecond must be tuple");
    typedef typename std::decay<decltype(std::tuple_cat(std::declval<TFirst>(), std::declval<TSecond>()))>::type Type;
};

template <typename TField, typename TTuple>
using TupleCatT = typename TupleCat<TField, TTuple>::Type;

}  // namespace util

}  // namespace comms