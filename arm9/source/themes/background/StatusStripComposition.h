#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace status_strip
{
constexpr int ScreenWidth = 256;
constexpr int ScreenHeight = 16;
constexpr size_t PixelCount = ScreenWidth * ScreenHeight;

enum class CompositionState
{
    Composed,
    Pristine,
    Invalid,
};

struct StatusCompositionResult
{
    CompositionState state;
    bool dirty;
};

struct StatusStripRequest
{
    std::span<const uint16_t> pristine;
    std::span<uint16_t> destination;
    std::span<const uint8_t> coverage;
    int left;
    int top;
    int width;
    int height;
    uint16_t foreground;
    bool restorePristine = true;
};

class StatusStripCompositor
{
public:
    static StatusCompositionResult Compose(const StatusStripRequest& request)
    {
        if (request.pristine.size() != PixelCount || request.destination.size() != PixelCount ||
            request.width < 0 || request.height < 0 ||
            request.coverage.size() != static_cast<size_t>(request.width) * request.height)
            return { CompositionState::Invalid, false };

        bool dirty = !std::equal(request.pristine.begin(), request.pristine.end(), request.destination.begin());
        if (request.restorePristine)
            std::copy(request.pristine.begin(), request.pristine.end(), request.destination.begin());

        const int left = std::max(request.left, 0);
        const int top = std::max(request.top, 0);
        const int right = std::min(request.left + request.width, ScreenWidth);
        const int bottom = std::min(request.top + request.height, ScreenHeight);
        bool composed = false;
        for (int y = top; y < bottom; ++y)
        {
            for (int x = left; x < right; ++x)
            {
                const uint8_t coverage = request.coverage[(y - request.top) * request.width + x - request.left];
                if (coverage == 0)
                    continue;
                uint16_t& destination = request.destination[y * ScreenWidth + x];
                destination = Blend(destination, request.foreground, coverage);
                composed = true;
            }
        }
        return { composed ? CompositionState::Composed : CompositionState::Pristine, dirty || composed };
    }

private:
    static constexpr uint16_t Blend(uint16_t destination, uint16_t foreground, uint8_t coverage)
    {
        const uint16_t alpha = std::min<uint8_t>(coverage, 15);
        const auto blendChannel = [alpha](uint16_t destinationChannel, uint16_t foregroundChannel)
        {
            return static_cast<uint16_t>((destinationChannel * (15 - alpha) + foregroundChannel * alpha + 7) / 15);
        };
        return static_cast<uint16_t>((destination & 0x8000) |
            blendChannel(destination & 0x1f, foreground & 0x1f) |
            (blendChannel((destination >> 5) & 0x1f, (foreground >> 5) & 0x1f) << 5) |
            (blendChannel((destination >> 10) & 0x1f, (foreground >> 10) & 0x1f) << 10));
    }
};
}
