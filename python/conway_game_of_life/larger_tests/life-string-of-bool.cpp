#include <algorithm>
#include <array>
#include <cstdio>
#include <type_traits>
#include <vector>
#include <fmt/format.h>

#include "parameters.hpp"

// necessary to keep the same algorithm as Python
constexpr auto floor_modulo(auto dividend, auto divisor)
{
  return ((dividend % divisor) + divisor) % divisor;
}

// if we wanted to push this further, we would make the width and height compile-time constants
// that is for a future episode.
struct Automata
{
  using index_t = std::make_signed_t<std::size_t>;

  std::size_t width;
  std::size_t height;
  std::array<bool, 9> born;
  std::array<bool, 9> survives;

  std::basic_string<bool> data = std::basic_string<bool>(width * height, false);

  constexpr Automata(std::size_t width_, std::size_t height_, std::array<bool, 9> born_, std::array<bool, 9> survives_)
    : width(width_), height(height_), born(born_), survives(survives_) {}

  struct Point
  {
    index_t x;
    index_t y;

    [[nodiscard]] constexpr Point operator+(Point rhs) const
    {
      return Point{ x + rhs.x, y + rhs.y };
    }
  };

  [[nodiscard]] constexpr std::size_t index(Point p) const
  {
    return floor_modulo(p.y, static_cast<index_t>(height)) * width + floor_modulo(p.x, static_cast<index_t>(width));
  }

  [[nodiscard]] constexpr bool get(Point p) const { return data[index(p)]; }

  constexpr void set(Point p) { data[index(p)] = true; }

  constexpr static std::array<Point, 8> neighbors{
    Point{ -1, -1 },
    Point{ 0, -1 },
    Point{ 1, -1 },
    Point{ -1, 0 },
    Point{ 1, 0 },
    Point{ -1, 1 },
    Point{ 0, 1 },
    Point{ 1, 1 }
  };

  constexpr std::size_t count_neighbors(Point p) const
  {
    return static_cast<std::size_t>(std::ranges::count_if(
      neighbors, [&](auto offset) { return get(p + offset); }));
  }

  [[nodiscard]] constexpr Automata next() const
  {
    Automata result{ width, height, born, survives };

    for (std::size_t y = 0; y < height; ++y) {
      for (std::size_t x = 0; x < width; ++x) {
        Point p{ static_cast<index_t>(x), static_cast<index_t>(y) };
        const auto neighbors = count_neighbors(p);
        if (get(p)) {
          if (survives[neighbors]) {
            result.set(p);
          }
        } else {
          if (born[neighbors]) {
            result.set(p);
          }
        }
      }
    }

    return result;
  }
  constexpr void add_glider(Point p)
  {
    set(p);
    set(p + Point(1, 1));
    set(p + Point(2, 1));
    set(p + Point(0, 2));
    set(p + Point(1, 2));
  }
};

int main()
{
  auto obj = Automata(WIDTH, HEIGHT, { false, false, false, true, false, false, false, false, false }, { false, false, true, true, false, false, false, false, false });

  obj.add_glider(Automata::Point(0, 18));

  for (int i = 0; i < ITERATIONS; ++i) {
    obj = obj.next();
  }

  for (size_t y = 0; y < obj.height; ++y) {
    for (size_t x = 0; x < obj.width; ++x) {
      if (obj.get(Automata::Point(static_cast<Automata::index_t>(x),
            static_cast<Automata::index_t>(y)))) {
        std::putchar('X');
      } else {
        std::putchar('.');
      }
    }
    std::puts("");
  }
}
