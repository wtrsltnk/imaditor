
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include "../image.h"

TEST_CASE("setSize works")
{
    Layer sut;

    sut.setSize(2, 2);

    for (int i = 0; i < sut.dataSize(); i++)
    {
        CHECK(sut._data[i] == 255);
    }
}

TEST_CASE("setPixel works")
{
    Layer sut;

    sut.setSize(2, 2);

    for (int i = 0; i < sut.dataSize(); i++)
    {
        CHECK(sut._data[i] == 255);
    }

    byte pixel[] = { 100, 150, 200, 255 };
    sut.setPixel<4>(0, 0, pixel);

    int i = 0;
    CHECK(sut._data[i++] == 100);
    CHECK(sut._data[i++] == 150);
    CHECK(sut._data[i++] == 200);
    CHECK(sut._data[i++] == 255);

    sut.setPixel<4>(1, 0, pixel);

    CHECK(sut._data[i++] == 100);
    CHECK(sut._data[i++] == 150);
    CHECK(sut._data[i++] == 200);
    CHECK(sut._data[i++] == 255);

    sut.setPixel<4>(0, 1, pixel);

    CHECK(sut._data[i++] == 100);
    CHECK(sut._data[i++] == 150);
    CHECK(sut._data[i++] == 200);
    CHECK(sut._data[i++] == 255);

    sut.setPixel<4>(1, 1, pixel);

    CHECK(sut._data[i++] == 100);
    CHECK(sut._data[i++] == 150);
    CHECK(sut._data[i++] == 200);
    CHECK(sut._data[i++] == 255);
}

TEST_CASE("overwrite layers works")
{
    Layer a;
    a.setSize(2, 2);

    Layer b;
    b.setSize(2, 2);

    byte pixel[] = { 100, 150, 200, 255 };
    b.setPixel<4>(0, 0, pixel);

    Layer::overwrite(&a, &b);

    CHECK(a._data[0] == 100);
    CHECK(a._data[1] == 150);
    CHECK(a._data[2] == 200);
    CHECK(a._data[3] == 255);

    for (int i = 4; i < 16; i++)
        CHECK(a._data[i] == 255);
}

TEST_CASE("overwrite layers with offsets works")
{
    Layer a;
    a.setSize(2, 2);
    a._offset[0] = a._offset[1] = 1;

    Layer b;
    b.setSize(2, 2);

    byte pixel[] = { 100, 150, 200, 255 };
    b.setPixel<4>(0, 0, pixel);
    b.setPixel<4>(1, 1, pixel);

    Layer::overwrite(&a, &b);

    CHECK(a._data[12] == 100);
    CHECK(a._data[13] == 150);
    CHECK(a._data[14] == 200);
    CHECK(a._data[15] == 255);

    for (int i = 0; i < 12; i++)
        CHECK(a._data[i] == 255);
}

TEST_CASE("overwrite layers with offsets out of image works")
{
    Layer a;
    a.setSize(2, 2);
    a._offset[0] = a._offset[1] = 2; // this moves a so far out that no changes are made when merged

    Layer b;
    b.setSize(2, 2);

    byte pixel[] = { 100, 150, 200, 255 };
    b.setPixel<4>(0, 0, pixel);
    b.setPixel<4>(1, 1, pixel);

    Layer::overwrite(&a, &b);

    for (int i = 0; i < 16; i++)
        CHECK(a._data[i] == 255);
}
