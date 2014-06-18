/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_TUPLE_PRIVATE_H
#define BLACKMISC_TUPLE_PRIVATE_H

#include <QtGlobal>
#include <QDBusArgument>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>
#include <QString>
#include <QJsonArray>
#include <QDebug>
#include <tuple>
#include <type_traits>
#include <functional>

namespace BlackMisc
{
    class CValueObject;

    namespace Private
    {

        // Inhibit doxygen warnings about missing documentation
        //! \cond PRIVATE

        // Using SFINAE to help detect missing BLACK_ENABLE_TUPLE_CONVERSION macro in static_assert
        std::false_type hasEnabledTupleConversionHelper(...);
        template <class T>
        typename T::EnabledTupleConversion hasEnabledTupleConversionHelper(T *);
        template <class T>
        struct HasEnabledTupleConversion
        {
            typedef decltype(hasEnabledTupleConversionHelper(static_cast<T *>(nullptr))) type;
            static const bool value = type::value;
        };

        // Using tag dispatch to select which implementation of compare() to use
        template <class T>
        int compareHelper(const T &a, const T &b, std::true_type isCValueObjectTag)
        {
            Q_UNUSED(isCValueObjectTag);
            return compare(a, b);
        }
        template <class T>
        int compareHelper(const T &a, const T &b, std::false_type isCValueObjectTag)
        {
            Q_UNUSED(isCValueObjectTag);
            if (a < b) { return -1; }
            if (a > b) { return 1; }
            return 0;
        }

        // Our own implementation of std::index_sequence (because not implemented by MSVC2013)
        template <size_t... Is>
        struct index_sequence
        {
            static const size_t size = sizeof...(Is);
            typedef std::tuple<std::integral_constant<size_t, Is>...> tuple_type;
        };
        template <size_t I, size_t C, size_t... Is>
        struct GenSequence
        {
            typedef typename GenSequence<I + 1, C, Is..., I>::type type;
        };
        template <size_t C, size_t... Is>
        struct GenSequence<C, C, Is...>
        {
            typedef index_sequence<Is...> type;
        };
        template <size_t C>
        using make_index_sequence = typename GenSequence<0, C>::type;

        // Create an index_sequence containing indices which match a given predicate.
        template <class P, size_t I, size_t C, bool B = false, size_t I2 = 0xDeadBeef, size_t... Is>
        struct GenSequenceOnPredicate;
        template <class P, size_t I, size_t C, size_t I2, size_t... Is>
        struct GenSequenceOnPredicate<P, I, C, true, I2, Is...>
        {
            static const bool test = P::template test<I>::value;
            typedef typename GenSequenceOnPredicate<P, I + 1, C, test, I, Is..., I2>::type type;
        };
        template <class P, size_t I, size_t C, size_t I2, size_t... Is>
        struct GenSequenceOnPredicate<P, I, C, false, I2, Is...>
        {
            static const bool test = P::template test<I>::value;
            typedef typename GenSequenceOnPredicate<P, I + 1, C, test, I, Is...>::type type;
        };
        template <class P, size_t C, size_t I2, size_t... Is>
        struct GenSequenceOnPredicate<P, C, C, true, I2, Is...>
        {
            typedef index_sequence<Is..., I2> type;
        };
        template <class P, size_t C, size_t I2, size_t... Is>
        struct GenSequenceOnPredicate<P, C, C, false, I2, Is...>
        {
            typedef index_sequence<Is...> type;
        };
        template <class P>
        using make_index_sequence_if = typename GenSequenceOnPredicate<P, 0, std::tuple_size<typename P::tuple_type>::value>::type;

        // Predicates used with make_index_sequence_if.
        template <quint64 F, class Tu>
        struct FlagPresent
        {
            typedef Tu tuple_type;
            template <size_t I>
            struct test : std::integral_constant<bool, bool(std::tuple_element<I, Tu>::type::flags & F)> {};
        };
        template <quint64 F, class Tu>
        struct FlagMissing
        {
            typedef Tu tuple_type;
            template <size_t I>
            struct test : std::integral_constant<bool, ! bool(std::tuple_element<I, Tu>::type::flags & F)> {};
        };

        // Combine make_index_sequence_if with predicates to get the indices of tuple elements with certain flags.
        template <quint64 F, class Tu>
        auto findFlaggedIndices(Tu &&) -> make_index_sequence_if<FlagPresent<F, typename std::decay<Tu>::type>>
        {
            return {};
        }
        template <quint64 F, class Tu>
        auto skipFlaggedIndices(Tu &&) -> make_index_sequence_if<FlagMissing<F, typename std::decay<Tu>::type>>
        {
            return {};
        }

        // A tuple element with attached metadata.
        template <class T, quint64 Flags = 0>
        struct Attribute
        {
            typedef T type;
            static const quint64 flags = Flags;

            Attribute(T &obj) : m_obj(obj) {}
            T &m_obj;

            bool operator ==(const Attribute &other) const { return m_obj == other.m_obj; }
            bool operator !=(const Attribute &other) const { return m_obj != other.m_obj; }
            bool operator <(const Attribute &other) const { return m_obj < other.m_obj; }
            bool operator <=(const Attribute &other) const { return m_obj <= other.m_obj; }
            bool operator >(const Attribute &other) const { return m_obj > other.m_obj; }
            bool operator >=(const Attribute &other) const { return m_obj >= other.m_obj; }
        };

        // Helpers used in tie(), tieMeta(), and elsewhere, which arrange for the correct types to be passed to std::make_tuple.
        // See http://en.cppreference.com/w/cpp/utility/tuple/make_tuple
        //     "For each Ti in Types..., the corresponding type Vi in Vtypes... is std::decay<Ti>::type
        //     unless application of std::decay results in std::reference_wrapper<X>, in which case the deduced type is X&."
        template <class T>
        std::reference_wrapper<T> tieHelper(T &obj)
        {
            return obj;
        }
        template <class T, quint64 Flags>
        std::reference_wrapper<T> tieHelper(Attribute<T, Flags> attr)
        {
            return attr.m_obj;
        }
        template <class T>
        Attribute<T> tieMetaHelper(T &obj)
        {
            return obj;
        }
        template <class T, quint64 Flags>
        Attribute<T, Flags> tieMetaHelper(Attribute<T, Flags> attr)
        {
            return attr;
        }

        // Convert a meta tuple to a value tuple
        template <class Tu, size_t... Is>
        auto stripMeta(Tu &&tu, index_sequence<Is...>) -> decltype(std::make_tuple(tieHelper(std::get<Is>(std::forward<Tu>(tu)))...))
        {
            return std::make_tuple(tieHelper(std::get<Is>(std::forward<Tu>(tu)))...);
        }

        // Convert a value tuple to a meta tuple with default metadata
        template <class Tu, size_t... Is>
        auto recoverMeta(Tu &&tu, index_sequence<Is...>) -> decltype(std::make_tuple(tieMetaHelper(std::get<Is>(std::forward<Tu>(tu)))...))
        {
            return std::make_tuple(tieMetaHelper(std::get<Is>(std::forward<Tu>(tu)))...);
        }

        // Applying operations to all elements in a tuple, using index_sequence for clean recursion
        class TupleHelper
        {
        public:
            template <class Tu, size_t... Is>
            static int compare(const Tu &a, const Tu &b, index_sequence<Is...>)
            {
                return compareImpl(std::make_pair(get_ref<Is>(a), get_ref<Is>(b))...);
            }

            template <class Tu, size_t... Is>
            static QDBusArgument &marshall(QDBusArgument &arg, const Tu &tu, index_sequence<Is...>)
            {
                marshallImpl(arg, std::get<Is>(tu)...);
                return arg;
            }

            template <class Tu, size_t... Is>
            static const QDBusArgument &unmarshall(const QDBusArgument &arg, Tu &tu, index_sequence<Is...>)
            {
                unmarshallImpl(arg, std::get<Is>(tu)...);
                return arg;
            }

            template <class Tu, size_t... Is>
            static QDebug debug(QDebug dbg, const Tu &tu, index_sequence<Is...>)
            {
                debugImpl(dbg, std::get<Is>(tu)...);
                return dbg;
            }

            template <class Tu, size_t... Is>
            static uint hash(const Tu &tu, index_sequence<Is...>)
            {
                return hashImpl(qHash(std::get<Is>(tu))...);
            }

            template <class Tu, size_t... Is>
            static void serializeJson(QJsonObject &json, const QStringList &names, const Tu &tu, index_sequence<Is...>)
            {
                serializeJsonImpl(json, std::make_pair(names[Is], std::get<Is>(tu))...);
            }

            template <class Tu, size_t... Is>
            static void deserializeJson(const QJsonObject &json, const QStringList &names, Tu &tu, index_sequence<Is...>)
            {
                deserializeJsonImpl(json, std::make_pair(names[Is], get_ref<Is>(tu))...);
            }

        private:
            template <size_t I, class T>
            static auto get_ref(T &&tu) -> decltype(std::ref(std::get<I>(std::forward<T>(tu))))
            {
                return std::ref(std::get<I>(std::forward<T>(tu)));
            }

            static int compareImpl() { return 0; }
            template <class T, class... Ts>
            static int compareImpl(const std::pair<T, T> &head, const Ts &... tail)
            {
                int result = compareHelper(head.first, head.second, typename std::is_base_of<CValueObject, typename std::decay<T>::type>::type());
                if (result) return result;
                return compareImpl(tail...);
            }

            static void marshallImpl(QDBusArgument &) {}
            template <class T, class... Ts>
            static void marshallImpl(QDBusArgument &arg, const T &head, const Ts &... tail)
            {
                arg << head;
                marshallImpl(arg, tail...);
            }

            static void unmarshallImpl(const QDBusArgument &) {}
            template <class T, class... Ts>
            static void unmarshallImpl(const QDBusArgument &arg, T &head, Ts &... tail)
            {
                arg >> head;
                unmarshallImpl(arg, tail...);
            }

            static void debugImpl(QDebug) {}
            template <class T, class... Ts>
            static void debugImpl(QDebug dbg, const T &head, const Ts &... tail)
            {
                dbg << head;
                debugImpl(dbg, tail...);
            }

            static void serializeJsonImpl(QJsonObject &) {}
            template <class T, class... Ts>
            static void serializeJsonImpl(QJsonObject &json, std::pair<QString, T> head, Ts... tail)
            {
                json << head;
                serializeJsonImpl(json, tail...);
            }

            static void deserializeJsonImpl(const QJsonObject &) {}
            template <class T, class... Ts>
            static void deserializeJsonImpl(const QJsonObject &json, std::pair<QString, T> head, Ts... tail)
            {
                json.value(head.first) >> head.second;
                deserializeJsonImpl(json, tail...);
            }

            static uint hashImpl() { return 0; }
            template <class... Ts>
            static uint hashImpl(uint head, Ts... tail) { return head ^ hashImpl(tail...); }
        };

        //! \endcond

    } // namespace Private

} // namespace BlackMisc

#endif // guard
