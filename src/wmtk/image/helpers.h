#pragma once

#include <wmtk/utils/Image.h>

#include <Eigen/Core>

#include <algorithm>
#include <array>
#include <tuple>

namespace wmtk::internal {

inline std::tuple<size_t, size_t, float> sample_coord(const float coord, const size_t size)
{
    assert(0.f <= coord && coord <= static_cast<float>(size));
    size_t coord0, coord1;
    float t;
    if (coord <= 0.5f) {
        coord0 = 0;
        coord1 = 0;
        t = 0.5f + coord;
    } else if (coord + 0.5f >= static_cast<float>(size)) {
        coord0 = size - 1;
        coord1 = size - 1;
        t = coord - (static_cast<float>(coord0) + 0.5f);
    } else {
        assert(1 < size);
        coord0 = std::min(size - 2, static_cast<size_t>(coord - 0.5f));
        coord1 = coord0 + 1;
        t = coord - (static_cast<float>(coord0) + 0.5f);
    }
    return std::make_tuple(coord0, coord1, t);
};

template <size_t N>
Eigen::Vector<float, N> sample_nearest(const std::array<wmtk::Image, N>& images, float u, float v)
{
    auto w = images[0].width();
    auto h = images[0].height();
    // x, y are between 0 and 1
    auto x = u * static_cast<float>(w);
    auto y = v * static_cast<float>(h);

    const auto sx = std::clamp(static_cast<int>(x), 0, w - 1);
    const auto sy = std::clamp(static_cast<int>(y), 0, h - 1);

    Eigen::Vector<float, N> res;
    for (size_t k = 0; k < N; ++k) {
        res[k] = images[k].get_raw_image()(sx, sy);
    }
    return res;
}

template <size_t N>
Eigen::Vector<float, N> sample_bilinear(const std::array<wmtk::Image, N>& images, float u, float v)
{
    auto w = images[0].width();
    auto h = images[0].height();
    // x, y are between 0 and 1
    auto x = u * static_cast<float>(w);
    auto y = v * static_cast<float>(h);

    const auto [x0, x1, tx] = sample_coord(x, w);
    const auto [y0, y1, ty] = sample_coord(y, h);

    const Eigen::Vector4f weight(
        (1.f - tx) * (1.f - ty),
        tx * (1.f - ty),
        (1.f - tx) * ty,
        tx * ty);

    Eigen::Vector<float, N> res;
    for (size_t k = 0; k < N; ++k) {
        const auto& array = images[k].get_raw_image();
        Eigen::Vector4f pix(array(x0, y0), array(x1, y0), array(x0, y1), array(x1, y1));
        res[k] = pix.dot(weight);
    }

    return res;
}

template <size_t N>
Eigen::Vector<float, N> sample_bicubic(const std::array<wmtk::Image, N>& images, float u, float v)
{
    auto w = images[0].width();
    auto h = images[0].height();
    // x, y are between 0 and 1
    auto x = u * static_cast<float>(w);
    auto y = v * static_cast<float>(h);

    // use bicubic interpolation
    Eigen::Vector<float, N> res;
    for (size_t k = 0; k < N; ++k) {
        BicubicVector<float> sample_vector = extract_samples(
            static_cast<size_t>(w),
            static_cast<size_t>(h),
            images[k].get_raw_image().data(),
            wmtk::get_value(x),
            wmtk::get_value(y),
            images[k].get_wrapping_mode_x(),
            images[k].get_wrapping_mode_y());
        BicubicVector<float> bicubic_coeff = get_bicubic_matrix() * sample_vector;
        res[k] = eval_bicubic_coeffs(bicubic_coeff, x, y);
    }
    return res;
}

} // namespace wmtk::internal