// Copyright 2021 The TensorStore Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "tensorstore/internal/compression/png.h"

#include <stddef.h>

#include <cmath>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "absl/status/status.h"
#include "absl/strings/cord.h"
#include "tensorstore/array.h"
#include "tensorstore/util/result.h"
#include "tensorstore/util/status.h"
#include "tensorstore/util/status_testutil.h"

namespace {

using tensorstore::MatchesStatus;
using tensorstore::png::Decode;
using tensorstore::png::Encode;
using tensorstore::png::EncodeOptions;

class PngCodingTest : public ::testing::TestWithParam<int> {
 public:
  EncodeOptions GetOptions() { return EncodeOptions{GetParam()}; }
};

INSTANTIATE_TEST_SUITE_P(WithOptions, PngCodingTest,
                         ::testing::Values(-1, 0, 6, 9));

TEST_P(PngCodingTest, OneComponent) {
  const size_t width = 100, height = 33, num_components = 1;

  std::vector<unsigned char> input_image(width * height * num_components);
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      double gradient =
          static_cast<double>(x + y) / static_cast<double>(width + height);
      input_image[y * width + x] = static_cast<unsigned char>(gradient * 255);
    }
  }

  absl::Cord encoded;
  TENSORSTORE_EXPECT_OK(Encode(input_image.data(), width, height,
                               num_components, GetOptions(), &encoded));

  std::vector<unsigned char> decoded(width * height * num_components);
  auto decode_status = Decode(encoded, [&](size_t w, size_t h, size_t num_c) {
    EXPECT_EQ(width, w);
    EXPECT_EQ(height, h);
    EXPECT_EQ(num_components, num_c);
    return decoded.data();
  });
  TENSORSTORE_EXPECT_OK(decode_status);

  EXPECT_THAT(decoded, testing::Eq(input_image));
}

TEST_P(PngCodingTest, TwoComponents) {
  const size_t width = 100, height = 33, num_components = 2;

  std::vector<unsigned char> input_image(width * height * num_components);
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      double gradient =
          static_cast<double>(x + y) / static_cast<double>(width + height);
      input_image[num_components * (y * width + x) + 0] =
          static_cast<unsigned char>(gradient * 255);
      input_image[num_components * (y * width + x) + 1] =
          static_cast<unsigned char>((1.0 - gradient) * 255);
    }
  }

  absl::Cord encoded;
  TENSORSTORE_EXPECT_OK(Encode(input_image.data(), width, height,
                               num_components, GetOptions(), &encoded));

  std::vector<unsigned char> decoded(width * height * num_components);
  auto decode_status = Decode(encoded, [&](size_t w, size_t h, size_t num_c) {
    EXPECT_EQ(width, w);
    EXPECT_EQ(height, h);
    EXPECT_EQ(num_components, num_c);
    return decoded.data();
  });
  TENSORSTORE_EXPECT_OK(decode_status);

  EXPECT_THAT(decoded, testing::Eq(input_image));
}

TEST_P(PngCodingTest, ThreeComponents) {
  const size_t width = 200, height = 33, num_components = 3;
  std::vector<unsigned char> input_image(width * height * num_components);
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      double gradient =
          static_cast<double>(x + y) / static_cast<double>(width + height);
      input_image[num_components * (y * width + x) + 0] =
          static_cast<unsigned char>(gradient * 255);
      input_image[num_components * (y * width + x) + 1] =
          static_cast<unsigned char>((1.0 - gradient) * 255);
      input_image[num_components * (y * width + x) + 2] =
          static_cast<unsigned char>(std::abs(128 - gradient * 255));
    }
  }

  absl::Cord encoded;
  TENSORSTORE_EXPECT_OK(Encode(input_image.data(), width, height,
                               num_components, GetOptions(), &encoded));

  std::vector<unsigned char> decoded(width * height * num_components);
  auto decode_status = Decode(encoded, [&](size_t w, size_t h, size_t num_c) {
    EXPECT_EQ(width, w);
    EXPECT_EQ(height, h);
    EXPECT_EQ(num_components, num_c);
    return decoded.data();
  });
  TENSORSTORE_EXPECT_OK(decode_status);
  EXPECT_THAT(decoded, testing::Eq(input_image));
}

TEST_P(PngCodingTest, FourComponents) {
  const size_t width = 200, height = 33, num_components = 4;
  std::vector<unsigned char> input_image(width * height * num_components);
  for (size_t y = 0; y < height; ++y) {
    for (size_t x = 0; x < width; ++x) {
      double gradient =
          static_cast<double>(x + y) / static_cast<double>(width + height);
      input_image[num_components * (y * width + x) + 0] = x;
      input_image[num_components * (y * width + x) + 1] =
          static_cast<unsigned char>(gradient * 255);
      input_image[num_components * (y * width + x) + 2] =
          static_cast<unsigned char>((1.0 - gradient) * 255);
      input_image[num_components * (y * width + x) + 3] =
          static_cast<unsigned char>(std::abs(128 - gradient * 255));
    }
  }

  absl::Cord encoded;
  TENSORSTORE_EXPECT_OK(Encode(input_image.data(), width, height,
                               num_components, GetOptions(), &encoded));

  std::vector<unsigned char> decoded(width * height * num_components);
  auto decode_status = Decode(encoded, [&](size_t w, size_t h, size_t num_c) {
    EXPECT_EQ(width, w);
    EXPECT_EQ(height, h);
    EXPECT_EQ(num_components, num_c);
    return decoded.data();
  });
  TENSORSTORE_EXPECT_OK(decode_status);
  EXPECT_THAT(decoded, testing::Eq(input_image));
}

TEST(PngTest, EncodeInvalidNumComponents) {
  const size_t width = 100, height = 33, num_components = 5;
  std::vector<unsigned char> input_image(width * height * num_components, 42);
  absl::Cord encoded;
  EncodeOptions options;
  EXPECT_THAT(
      Encode(input_image.data(), width, height, num_components, options,
             &encoded),
      MatchesStatus(absl::StatusCode::kInvalidArgument,
                    ".*PNG encoding requires between 1 and 4 components"));
}

TEST(PngTest, TinyRGB) {
  // https://shoonia.github.io/1x1/#5f5f5fff
  static constexpr unsigned char data[] = {
      0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,  // sig
      0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x02,
      0x00, 0x00, 0x00, 0x90, 0x77, 0x53,
      0xde,  // ihdr
      0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 0x42, 0x00,
      0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00, 0x00, 0x0c, 0x49,
      0x44, 0x41, 0x54, 0x18, 0x57, 0x63, 0x88, 0x8f, 0x8f,
      0x07, 0x00, 0x02, 0x3e, 0x01, 0x1e, 0x78, 0xd8, 0x99,
      0x68,  // idat
      0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
      0x42, 0x60, 0x82,  // iend
  };
  unsigned char pixel[4] = {0, 0, 0, 0};
  auto decode_status =
      Decode(absl::Cord(absl::string_view(reinterpret_cast<const char*>(data),
                                          sizeof(data))),
             [&](size_t w, size_t h, size_t num_c) {
               EXPECT_EQ(1, w);
               EXPECT_EQ(1, h);
               EXPECT_EQ(3, num_c);
               return pixel;
             });
  EXPECT_EQ(95, pixel[0]);
  EXPECT_EQ(95, pixel[1]);
  EXPECT_EQ(95, pixel[2]);
  EXPECT_EQ(0, pixel[3]);
}

TEST(PngTest, OneBitPng) {
  static constexpr unsigned char data[] = {
      /* SIG*/ 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a,
      /*IHDR*/ 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, /**/
      /*....*/ 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
      /*....*/ 0x00, 0x00, 0x00, 0x00, /**/
      /*....*/ 0x37, 0x6e, 0xf9, 0x24,
      /*IDAT*/ 0x00, 0x00, 0x00, 0x10, 0x49, 0x44, 0x41, 0x54, /**/
      /*....*/ 0x78, 0x9c, 0x62, 0x60, 0x01, 0x00, 0x00, 0x00, 0xff,
      /*....*/ 0xff, 0x03, 0x00, 0x00, 0x06, 0x00, 0x05, /**/
      /*....*/ 0x57, 0xbf, 0xab, 0xd4,
      /*IEND*/ 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, /**/
      /*....*/ 0xae, 0x42, 0x60, 0x82,
  };

  unsigned char pixel[2] = {255, 255};
  auto decode_status =
      Decode(absl::Cord(absl::string_view(reinterpret_cast<const char*>(data),
                                          sizeof(data))),
             [&](size_t w, size_t h, size_t num_c) {
               // never reached
               return pixel;
             });

  EXPECT_THAT(decode_status, MatchesStatus(absl::StatusCode::kUnimplemented));
}

}  // namespace
