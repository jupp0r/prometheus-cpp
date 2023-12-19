#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace prometheus {
namespace detail {

/*
Copyright (C) 2019-2020 by Martin Vorbrodt <martin@vorbrodt.blog>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

https://github.com/mvorbrodt/blog/blob/master/src/base64.hpp
*/

static constexpr char kEncodeLookup[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static constexpr char kPadCharacter = '=';

inline std::string base64_encode(const std::string& input) {
  std::string encoded;
  encoded.reserve(((input.size() / 3) + (input.size() % 3 > 0)) * 4);

  std::uint32_t temp{};
  auto it = input.begin();

  for (std::size_t i = 0; i < input.size() / 3; ++i) {
    temp = static_cast<std::uint8_t>(*it++) << 16;
    temp += static_cast<std::uint8_t>(*it++) << 8;
    temp += static_cast<std::uint8_t>(*it++);
    encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
    encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
    encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
    encoded.append(1, kEncodeLookup[(temp & 0x0000003F)]);
  }

  switch (input.size() % 3) {
    case 1:
      temp = static_cast<std::uint8_t>(*it++) << 16;
      encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
      encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
      encoded.append(2, kPadCharacter);
      break;
    case 2:
      temp = static_cast<std::uint8_t>(*it++) << 16;
      temp += static_cast<std::uint8_t>(*it++) << 8;
      encoded.append(1, kEncodeLookup[(temp & 0x00FC0000) >> 18]);
      encoded.append(1, kEncodeLookup[(temp & 0x0003F000) >> 12]);
      encoded.append(1, kEncodeLookup[(temp & 0x00000FC0) >> 6]);
      encoded.append(1, kPadCharacter);
      break;
  }

  return encoded;
}

// https://tools.ietf.org/html/rfc4648#section-5
inline std::string base64url_encode(const std::string& input) {
  std::string s = base64_encode(input);
  std::transform(begin(s), end(s), begin(s), [](char c) {
    if (c == '+') return '-';
    if (c == '/') return '_';
    return c;
  });
  return s;
}

inline std::string base64_decode(const std::string& input) {
  if (input.length() % 4) {
    throw std::runtime_error("Invalid base64 length!");
  }

  std::size_t padding = 0;

  if (!input.empty()) {
    if (input[input.length() - 1] == kPadCharacter) padding++;
    if (input[input.length() - 2] == kPadCharacter) padding++;
  }

  std::string decoded;
  decoded.reserve(((input.length() / 4) * 3) - padding);

  std::uint32_t temp = 0;
  auto it = input.begin();

  while (it < input.end()) {
    for (std::size_t i = 0; i < 4; ++i) {
      temp <<= 6;
      if (*it >= 0x41 && *it <= 0x5A) {
        temp |= *it - 0x41;
      } else if (*it >= 0x61 && *it <= 0x7A) {
        temp |= *it - 0x47;
      } else if (*it >= 0x30 && *it <= 0x39) {
        temp |= *it + 0x04;
      } else if (*it == 0x2B) {
        temp |= 0x3E;
      } else if (*it == 0x2F) {
        temp |= 0x3F;
      } else if (*it == kPadCharacter) {
        switch (input.end() - it) {
          case 1:
            decoded.push_back((temp >> 16) & 0x000000FF);
            decoded.push_back((temp >> 8) & 0x000000FF);
            return decoded;
          case 2:
            decoded.push_back((temp >> 10) & 0x000000FF);
            return decoded;
          default:
            throw std::runtime_error("Invalid padding in base64!");
        }
      } else {
        throw std::runtime_error("Invalid character in base64!");
      }

      ++it;
    }

    decoded.push_back((temp >> 16) & 0x000000FF);
    decoded.push_back((temp >> 8) & 0x000000FF);
    decoded.push_back((temp) & 0x000000FF);
  }

  return decoded;
}

}  // namespace detail
}  // namespace prometheus
