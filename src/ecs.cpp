#include <tuple>
#include <bitset>


template <int N, typename ... Components>
class Pool
{
    static const std::size_t value = N;
    using type = std::tuple<Components...>;

    Pool* next;

    std::bitset<N> killed;
    std::size_t count;
    std::tuple<Components[N]...> data;
    bool dirty;

public:
    template <typename T>
    [[nodiscard]] inline T& get(std::size_t i) const noexcept
    {
        return std::get<T[N]>(this->data)[i];
    }
    template <typename T>
    [[nodiscard]] inline void assign(std::size_t i, T&& x) const noexcept
    {
        auto& component = this->get<T>(i);
        component = x;
    }
    std::size_t create(Components&& ... components)
    {
        (this->assign(this->count, components), ...);
        return this->count++;
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
        for (std::size_t i = 0; i < this->count; ++i)
        {
            if (this->killed[i])
            {
                // TODO(ted): @BUG Need to check the one we switched for also.
                auto& a = std::make_tuple(this->get<Components>(i)...);
                auto& b = std::make_tuple(this->get<Components>(this->count-1)...);

                auto& temp = a;
                a = b;
                b = temp;

                this->count -= 1;
                this->killed[i] = false;
            }
        }
    }
};



template <typename ... Pools>
class World
{
    std::tuple<Pools...> pools;

    template <typename T>
    struct Iterator
    {



    };

public:

    template <typename ... Components>
    std::size_t create_entity(Components&& ... components)
    {
        auto& pool   = std::get<Pool<1024, Components...>>(this->pools);
        auto  entity = pool.create(std::forward<Components>(components)...);
        return entity;
    }

};

template <typename ... Components>
struct Group {};


struct Position { float a, b, c; };
struct Velocity { float a, b, c; };
struct Scale    { float a, b, c; };
struct Hitbox   { int x, y; };
struct Tag      { };



int main()
{
    using A = Pool<1024, Position, Velocity, Scale>;
    using B = Pool<1024, Position, Velocity, Scale, Hitbox, Tag>;
    using C = Pool<1024, Position, Tag>;
    using D = Pool<1024, Position, Hitbox>;
    using E = Pool<1024, Position, Scale, Hitbox>;
    using F = Pool<1024, Position, Velocity>;

    World<A, B, C, D, E, F> world;


    auto a = world.create_entity<Position, Velocity, Scale>({0, 0, 0}, {0, 0, 0}, {0, 0, 0});

}