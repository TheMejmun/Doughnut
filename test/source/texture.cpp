//
// Created by Saman on 08.04.24.
//

#include "test_util.h"
#include "graphics/texture.h"

bool isSimilar(double x, double y, double thresholdNormalized) {
    return (std::abs(x - y) / std::max(std::abs(x), std::abs(y))) <= thresholdNormalized;
}

REGISTER(Texture) {
    TEST(convertType) {
                          auto *data = (uint8_t *) std::malloc(sizeof(uint8_t));
                          data[0] = 128;

                          dn::Texture test0{
                                  "test",
                                  data,
                                  1,
                                  1,
                                  dn::TextureLayout{
                                          1,
                                          dn::MONO,
                                          dn::LINEAR_UINT
                                  }
                          };
                          std::free(data);
                          expect(((uint8_t *) test0.mData)[0] == 128, "Initial value should be copied correctly");

                          auto test1 = test0.convertTo(dn::TextureLayout{
                                  4,
                                  dn::MONO,
                                  dn::LINEAR_FLOAT
                          });
                          expect(
                                  isSimilar(((float *) test1.mData)[0], 0.5, 0.01),
                                  "Conversion error 0 -> 1 must fall within tolerance"
                          );

                          auto test2 = test1.convertTo(dn::TextureLayout{
                                  2,
                                  dn::MONO,
                                  dn::LINEAR_UINT
                          });
                          expect(
                                  isSimilar(((uint16_t *) test2.mData)[0], 32768, 0.01),
                                  "Conversion error 1 -> 2 must fall within tolerance"
                          );

                          auto test3 = test2.convertTo(dn::TextureLayout{
                                  8,
                                  dn::MONO,
                                  dn::LINEAR_FLOAT
                          });
                          expect(
                                  isSimilar(((double *) test3.mData)[0], 0.5, 0.01),
                                  "Conversion error 2 -> 3 must fall within tolerance"
                          );

                          auto test4 = test3.convertTo(dn::TextureLayout{
                                  4,
                                  dn::MONO,
                                  dn::LINEAR_UINT
                          });
                          expect(
                                  isSimilar(((uint32_t *) test4.mData)[0], 2147483648, 0.01),
                                  "Conversion error 3 -> 4 must fall within tolerance"
                          );

                          auto test5 = test4.convertTo(dn::TextureLayout{
                                  1,
                                  dn::MONO,
                                  dn::LINEAR_UINT
                          });
                          expect(
                                  isSimilar(((uint8_t *) test5.mData)[0], 128, 0.01),
                                  "Conversion error 4 -> 5 must fall within tolerance"
                          );
                      };
}