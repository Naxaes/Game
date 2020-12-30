#include <tuple>
#include <bitset>
#include <utility>


std::size_t GenerateID()
{
    static std::size_t i = 0;
    return i++;
}

template <typename ... T>
std::size_t GetID()
{
    static std::size_t i = GenerateID();
    return i;
}


template <typename ... T>
struct Entity
{
    std::size_t group_id;
    std::size_t id;
};


template <typename ... Components>
class View
{
    class Iterator
    {
        using Self = Iterator;

        std::tuple<Components*...> data;
        std::size_t i;
        std::size_t group;

    public:
        Iterator(std::tuple<Components*...> data, std::size_t i, std::size_t group) noexcept
                : data{std::move(data)}, i{i}, group{group} {}

        auto operator* ()
        {
            return std::make_tuple(
                Entity<Components...>{group, i}, std::get<Components*>(this->data)[i]...
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
    std::size_t group;

public:
    using iterator_type = Iterator;

    explicit View(std::tuple<Components*...> data, std::size_t count, std::size_t group)
        : data{std::move(data)}, count{count}, group{group} {}

    iterator_type begin()
    {
        return Iterator{ data, 0, group };
    }
    iterator_type end()
    {
        return Iterator{ data, count, group };
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
    std::size_t id;
    std::tuple<Components*...> data;
    bool dirty;


public:
    Pool() : next(nullptr), killed({}), dirty(false), _count(0), id{GetID<Components...>()}, data({})
    {
        ((std::get<Components*>(this->data) = new Components[N]), ...);
    }
    template <typename ... T>
    [[nodiscard]] inline View<T...> get() const noexcept
    {
        auto components = std::make_tuple(std::get<T*>(this->data)...);
        return View{components, this->_count, this->id};
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



void System1(View<Position, Velocity, Scale> view)
{
    for (const auto& [entity, position, velocity, scale] : view)
    {
        printf("\nEntity:   %lu, %lu\n", entity.group_id, entity.id);
        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
        printf("Velocity: %f, %f, %f\n", velocity.a, velocity.b, velocity.c);
        printf("Scale:    %f, %f, %f\n", scale.a, scale.b, scale.c);
    }
}
void System2(View<Position, Velocity> view)
{
    for (const auto& [entity, position, velocity] : view)
    {
        printf("\nEntity:   %lu, %lu\n", entity.group_id, entity.id);
        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
        printf("Velocity: %f, %f, %f\n", velocity.a, velocity.b, velocity.c);
    }
}

void System3(View<Position> view)
{
    for (const auto& [entity, position] : view)
    {
        printf("\nEntity:   %lu, %lu\n", entity.group_id, entity.id);
        printf("Position: %f, %f, %f\n", position.a, position.b, position.c);
    }
}

int main()
{
    using A = Pool<1024, Position, Velocity, Scale>;
    using B = Pool<1024, Position, Velocity, Scale, Hitbox, Tag>;
    using C = Pool<1024, Position, Tag>;
    using D = Pool<1024, Position, Hitbox>;
    using E = Pool<1024, Position, Scale, Hitbox>;
    using F = Pool<1024, Position, Velocity>;

    A pool1;
    B pool2;
    C pool3;
    D pool4;
    E pool5;
    F pool6;
    for (int i = 0; i < 30; ++i)
    {
        if (i < 5)
            pool1.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {1.4f*float(i),1.5f*float(i),1.6f*float(i)},
                {1.7f*float(i),1.8f*float(i),1.9f*float(i)}
            );
        else if (i < 10)
            pool2.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {1.4f*float(i),1.5f*float(i),1.6f*float(i)},
                {1.7f*float(i),1.8f*float(i),1.9f*float(i)},
                {i, i+1},
                {}
            );
        else if (i < 15)
            pool3.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {}
            );
        else if (i < 20)
            pool4.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {i, i+1}
            );
        else if (i < 25)
            pool5.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {1.7f*float(i),1.8f*float(i),1.9f*float(i)},
                {i, i+1}
            );
        else
            pool6.create(
                {1.1f*float(i),1.2f*float(i),1.3f*float(i)},
                {1.4f*float(i),1.5f*float(i),1.6f*float(i)}
            );
    }

    System1(pool1.get<Position, Velocity, Scale>());
    System1(pool2.get<Position, Velocity, Scale>());

    System2(pool1.get<Position, Velocity>());
    System2(pool2.get<Position, Velocity>());
    System2(pool6.get<Position, Velocity>());

    System3(pool1.get<Position>());
    System3(pool2.get<Position>());
    System3(pool3.get<Position>());
    System3(pool4.get<Position>());
    System3(pool5.get<Position>());
    System3(pool6.get<Position>());
}