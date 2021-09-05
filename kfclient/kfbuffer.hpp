#ifndef kfclient_buffer_hpp
#define kfclient_buffer_hpp

#include <stdexcept>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <utility>
#include <string>

#include "libdef.hpp"

namespace kfc {
    struct KFCLIENT_API KFCLIENT_PACKED kfheader {
        std::int32_t magic;
        char type;
    };

    class KFCLIENT_API kfbuffer {
    public:
        template <std::size_t _Size>
        kfbuffer(std::uint8_t(&data)[_Size]) 
            : allocated_(false), data_(data), size_(size), pos_(0) {}
        
        kfbuffer(std::uint8_t* data, std::size_t size)
            : allocated_(false), data_(data), size_(size), pos_(0) {}
        
        kfbuffer(std::size_t size)
            : allocated_(true), data_(new std::uint8_t[size]), size_(size), pos_(0) {}
        
        ~kfbuffer() {
            if (allocated_)
                delete[] data_;
        }

        std::uint8_t* data() noexcept { return data_; }
        const std::uint8_t* data() const noexcept { return data_; }
        std::size_t size() const noexcept { return size_; }

        void rewind() const { 
            pos_ = 0;
        }

        void seek(std::streamoff offset, std::ios::seekdir dir = std::ios::cur) const {
            std::streamoff result;

            switch (dir) {
            case std::ios::beg:
                result = offset;
                break;
            case std::ios::cur:
                result = static_cast<std::streamoff>(pos_) + offset;
                break;
            case std::ios::end:
                result = static_cast<std::streamoff>(size_) - offset;
                break;
            }

            if (result < 0 || result > static_cast<std::streamoff>(size_))
                throw std::range_error("seeking outside of available memory");
            
            pos_ = static_cast<std::size_t>(result);
        }

        template <typename T>
        const T& consume() const {
            auto cur = pos_;
            seek(sizeof(T));
            return *static_cast<const T*>(static_cast<const void*>(data_ + cur));
        }

        template <typename T, typename C>
        C consume_cast() const {
            return static_cast<C>(consume<T>());
        }

        std::string consume_string_null() const {
            auto cur = pos_;
            for (auto pos = cur; pos < static_cast<std::streamoff>(size_); ++pos) {
                if (data_[pos] == 0) {
                    seek(pos - cur + 1);
                    return std::string(static_cast<const char*>(static_cast<const void*>(data_ + cur)), static_cast<std::size_t>(pos - cur));
                }
            }

            throw std::range_error("consume_string is trying to read outside of the available memory, NUL character not found.");
        }

        std::string consume_string(std::size_t length) const {
            auto cur = pos_;
            seek(length);
            return std::string(static_cast<const char*>(static_cast<const void*>(data_ + cur)), length);
        }

        template <typename T>
        void consume(T& v) const {
            v = consume<T>();
        }

        template <typename ... T>
        void consume(T&&... vs) const {
            (consume(std::forward<T>(vs)), ...);
        }

        template <typename T, typename C>
        void consume_cast(C& v) const {
            v = consume_cast<T, C>();
        }

        void consume(std::string& v) const {
            v = consume_string_null();
        }

        void consume_string(std::string& v, std::size_t length) const {
            v = consume_string(length);
        }
    private:
        bool allocated_;
        std::uint8_t* data_;
        std::size_t size_;
        mutable std::size_t pos_;
    };
}

#endif