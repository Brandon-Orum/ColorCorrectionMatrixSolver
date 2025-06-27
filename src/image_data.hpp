#pragma once

struct ImageData {
    std::unique_ptr<unsigned char, void(*)(void*)> data;
    int width;
    int height;
    int channels;

    ImageData() : data(nullptr, nullptr), width(0), height(0), channels(0) {}

    ImageData(ImageData&& other) noexcept
        : data(std::move(other.data)), width(other.width), height(other.height), channels(other.channels)
    {
        other.width = 0;
        other.height = 0;
        other.channels = 0;
    }

    ImageData& operator=(ImageData&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            width = other.width;
            height = other.height;
            channels = other.channels;
            other.width = 0;
            other.height = 0;
            other.channels = 0;
        }
        return *this;
    }

    ImageData(const ImageData&) = delete;
    ImageData& operator=(const ImageData&) = delete;

    bool isValid() const {
        return data != nullptr && width > 0 && height > 0 && channels > 0;
    }
};