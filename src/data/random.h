#ifndef CSOPESY_SRC_DATA_RANDOM_H_
#define CSOPESY_SRC_DATA_RANDOM_H_

#include <cstdint>

namespace csopesy::data {

// SplitMix64, a small random number generator written out here so the same
// seed gives the same numbers with every compiler and standard library
// (std:: distributions differ between them).
class SplitMix64 {
 public:
  explicit constexpr SplitMix64(std::uint64_t seed) : state_(seed) {}

  constexpr std::uint64_t Next() {
    state_ += kIncrement;
    std::uint64_t mixed = state_;
    mixed = (mixed ^ (mixed >> kShift1)) * kMultiplier1;
    mixed = (mixed ^ (mixed >> kShift2)) * kMultiplier2;
    return mixed ^ (mixed >> kShift3);
  }

  // Uniform in [0, bound). The modulo bias is negligible for small bounds.
  constexpr std::uint64_t NextBelow(std::uint64_t bound) {
    return bound == 0 ? 0 : Next() % bound;
  }

  // Uniform in [-1, 1].
  constexpr double NextSigned() {
    const double unit =
        static_cast<double>(Next() >> kUnitShift) * kUnitScale;  // [0, 1)
    return unit + unit - 1.0;
  }

 private:
  static constexpr std::uint64_t kIncrement = 0x9E3779B97F4A7C15ULL;
  static constexpr std::uint64_t kMultiplier1 = 0xBF58476D1CE4E5B9ULL;
  static constexpr std::uint64_t kMultiplier2 = 0x94D049BB133111EBULL;
  static constexpr unsigned kShift1 = 30;
  static constexpr unsigned kShift2 = 27;
  static constexpr unsigned kShift3 = 31;
  // Keeps the top 53 bits, the precision of a double.
  static constexpr unsigned kUnitShift = 11;
  static constexpr double kUnitScale = 1.0 / 9007199254740992.0;  // 2^-53

  std::uint64_t state_;
};

}  // namespace csopesy::data

#endif  // CSOPESY_SRC_DATA_RANDOM_H_
