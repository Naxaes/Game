#include <tuple>
#include <array>
#include <utility>
#include <type_traits>
#include <iostream>


template <typename T, typename... Ts>
constexpr bool contains = (std::is_same<T, Ts>{} || ...);

template <typename Subset, typename Set>
constexpr bool is_subset_of = false;

template <typename... Ts, typename... Us>
constexpr bool is_subset_of<std::tuple<Ts...>, std::tuple<Us...>> = (contains<Ts, Us...> && ...);

template <typename ... T>
struct View
{
    std::tuple<T...> data;
};

template <typename ... T>
struct Group
{
    using type_set = std::tuple<T...>;

    static const char* name;

    std::tuple<T...> data;

    explicit Group(T... x) : data(x...) {}

    template <typename ... U>
    [[nodiscard]] View<U...> get() const noexcept
    {
        return { std::make_tuple(std::get<U>(data)...) };
    }
};


template <class GroupSequence, std::size_t i, class ... Types>
constexpr bool is_match()
{
    using group_type = std::tuple_element_t<i, GroupSequence>;
    bool match = is_subset_of<std::tuple<Types...>, typename group_type::type_set>;
    return match;
}


template <std::size_t i, class ... Types, class GroupSequence, class Array>
constexpr void add_matches(const GroupSequence& candidates, Array& matches, std::size_t& index)
{
    using group_type = std::tuple_element_t<i, GroupSequence>;
    if constexpr (is_match<GroupSequence, i, Types...>())
    {
        std::cout << group_type::name << std::endl;

        auto& group = std::get<i>(candidates);
        matches[index++] = group.template get<Types...>();
    }
}

template <class ... Types, class GroupSequence, std::size_t ... indices>
constexpr auto get_matches(const GroupSequence& candidates, std::index_sequence<indices...>)
{
    constexpr std::size_t size = (is_match<GroupSequence, indices, Types...>() + ... + 0);
    std::array<View<Types...>, size> views {};
    std::size_t index = 0;
    (add_matches<indices, Types...>(candidates, views, index), ...);
    return views;
}


template <typename ... Group>
class Registry
{
public:
    explicit Registry(Group... groups) : groups(groups...) {}

    template <typename ... T>
    auto get()
    {
        constexpr auto indices = std::index_sequence_for<Group...>{};
        return get_matches<T...>(this->groups, indices);
    }

private:
    std::tuple<Group...> groups;
};


using A = Group<char>;
template<> const char* A::name = "A";
using B = Group<char, short>;
template<> const char* B::name = "B";
using C = Group<char, short, int>;
template<> const char* C::name = "C";
using D = Group<char, short, int, long long>;
template<> const char* D::name = "D";

int main()
{
    Registry<A, B, C, D> registry(A{0}, B{1,1}, C{2,2,2}, D{3,3,3,3});

    // Should yield an array of size 2 with View<char, short, int>,
    // one from group C and one from Group D.
    auto views = registry.get<char, short, int>();
    for (const auto& view : views)
    {
        std::cout << "char: "  << (int)std::get<char>(view.data)  << std::endl;
        std::cout << "short: " << std::get<short>(view.data) << std::endl;
        std::cout << "int: "   << std::get<int>(view.data)   << std::endl;
    }
}




