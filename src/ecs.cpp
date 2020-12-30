#include <tuple>
#include <bitset>
#include <utility>

template <typename ... T>
struct Entity
{
    std::size_t id;
};


template <typename ... Components>
class View
{
    class Iterator
    {
        using Self = Iterator;

        std::size_t i;
        std::tuple<Components*...> data;

    public:
        Iterator(std::size_t i, std::tuple<Components*...> data) noexcept
                : i(i), data(std::move(data)) {}

        auto operator* ()
        {
            return std::make_tuple(
                Entity<Components...>{i}, std::get<Components*>(this->data)[i]...
            );
        }

        Self& operator++() noexcept {
            ++this->i;
            return *this;
        }

        Self operator++(int) noexcept {
            Self orig = *this;
            return this->operator++(), orig;
        }

        bool operator==(const Self& other) const noexcept {
            return this->i == other.i;
        }

        bool operator!=(const Self &other) const noexcept {
            return !(*this == other);
        }
    };

    std::tuple<Components*...> data;
    std::size_t count;

public:
    using iterator_type = Iterator;

    explicit View(std::tuple<Components*...> data, std::size_t count)
        : data{std::move(data)}, count{count} {}

    iterator_type begin()
    {
        return Iterator{ 0, data };
    }
    iterator_type end()
    {
        return Iterator{ count, data };
    }
};



template <int N, typename ... Components>
class Pool
{
    using Self = Pool<N, Components...>;

    static constexpr std::size_t size = N;

    Pool* next;

    std::bitset<N> killed;
    std::size_t _count;
    std::tuple<Components*...> data;
    bool dirty;


public:
    Pool() : next(nullptr), killed({}), dirty(false), _count(0), data({})
    {
        ((std::get<Components*>(this->data) = new Components[N]), ...);
    }
    template <typename ... T>
    [[nodiscard]] inline View<T...> get() const noexcept
    {
        auto components = std::make_tuple(std::get<T*>(this->data)...);
        return View{components, this->_count};
    }
    template <typename T>
    [[nodiscard]] inline T& get(std::size_t i) const noexcept
    {
        using value_type = typename std::remove_reference<T>::type;
        auto  array = std::get<value_type*>(this->data);
        return array[i];
    }
    template <typename T>
    inline void assign(std::size_t i, T&& component) noexcept
    {
        this->get<T>(i) = component;
    }
    Entity<Components...> inline create(Components&& ... components)
    {
        if (this->_count >= N) throw std::runtime_error("Pool exceeded!");
        ((this->get<Components>(this->_count) = components), ...);
        return {this->_count++};
    }
    [[nodiscard]] inline std::size_t count() const noexcept
    {
        return this->_count;
    }
    // TODO(ted): Provide `kill_immediate`? Can't swap as it'll invalidate
    //  iterators.
    void kill(std::size_t i)
    {
        dirty = true;
        this->killed[i] = true;
    }
    void tick()
    {
        if (!this->dirty)
            return;

        // TODO(ted): Can be faster with packed bitset and just checking if
        //  if the chunk of 32/64 (or more) bits evaluates to true.
        for (std::size_t i = 0; i < this->_count; ++i)
        {
            if (this->killed[i])
            {
                // TODO(ted): @BUG Need to check the one we switched for also.
                auto& a = std::make_tuple(this->get<Components>(i)...);
                auto& b = std::make_tuple(this->get<Components>(this->_count-1)...);

                auto& temp = a;
                a = b;
                b = temp;

                this->_count -= 1;
                this->killed[i] = false;
            }
        }
    }
};



struct Position { float a, b, c; };
struct Velocity { float a, b, c; };
struct Scale    { float a, b, c; };
struct Hitbox   { int x, y; };
struct Tag      { };



//void System()
//{
//    for (std::size_t i = 0; i < pool1.count(); ++i)
//    {
//        auto position = pool1.get<Position>(i);
//        auto velocity = pool1.get<Velocity>(i);
//        auto scale    = pool1.get<Scale>(i);
//
//        printf("\nEntity:   %lu\n", i);
//        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
//        printf("Velocity: %f, %f, %f\n", velocity.a, velocity.b, velocity.c);
//        printf("Scale:    %f, %f, %f\n", scale.a, scale.b, scale.c);
//    }
//}

int main()
{
    using A = Pool<1024, Position, Velocity, Scale>;
    using B = Pool<1024, Position, Velocity, Scale, Hitbox, Tag>;
    using C = Pool<1024, Position, Tag>;
    using D = Pool<1024, Position, Hitbox>;
    using E = Pool<1024, Position, Scale, Hitbox>;
    using F = Pool<1024, Position, Velocity>;

    A pool1;
    for (int i = 0; i < 1024; ++i)
    {
        pool1.create(
            {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
            {1.4f*float(i),1.5f*float(i),1.6f*float(i)},
            {1.7f*float(i),1.8f*float(i),1.9f*float(i)}
        );
    }

//    for (std::size_t i = 0; i < pool1.count(); ++i)
//    {
//        auto position = pool1.get<Position>(i);
//        auto velocity = pool1.get<Velocity>(i);
//        auto scale    = pool1.get<Scale>(i);
//
//        printf("\nEntity:   %lu\n", i);
//        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
//        printf("Velocity: %f, %f, %f\n", velocity.a, velocity.b, velocity.c);
//        printf("Scale:    %f, %f, %f\n", scale.a, scale.b, scale.c);
//    }

    for (const auto& [entity, position, velocity, scale] : pool1.get<Position, Velocity, Scale>())
    {
        printf("\nEntity:   %lu\n", entity.id);
        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
        printf("Velocity: %f, %f, %f\n", velocity.a, velocity.b, velocity.c);
        printf("Scale:    %f, %f, %f\n", scale.a, scale.b, scale.c);
    }
}