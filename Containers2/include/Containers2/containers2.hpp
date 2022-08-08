#pragma once

#include <array>
#include <type_traits>

namespace containers2 {

    template<typename T>
    struct VectorView;

    template<typename T>
    struct Vector;

    template<typename T> requires(std::is_const_v<T>)
    struct VectorView<T> 
    {
        T* begin_ = nullptr;
        T* end_ = nullptr;

        template<size_t N>
        VectorView(T(&arr)[N]) noexcept : begin_{ std::begin(arr) }, end_{ std::end(arr) } {}
        VectorView(T* begin, T* end) noexcept : begin_{ begin }, end_{ end } {}

        VectorView() noexcept = default;

        VectorView(const VectorView&) noexcept = default;
        VectorView& operator = (const VectorView&) & noexcept = default;

        VectorView(VectorView&&) noexcept = default;
        VectorView& operator = (VectorView&&) & noexcept = default;

        ~VectorView() noexcept = default;

        VectorView(Vector<T>&&) = delete;
        VectorView& operator = (Vector<T>&&) = delete;
        VectorView(Vector<std::remove_const_t<T>>&&) = delete;
        VectorView& operator = (Vector<std::remove_const_t<T>>&&) = delete;

        T* begin() const { return begin_; }
        T* end() const { return end_; }

        T* data() const { return begin_; }

        size_t size() const { return end() - begin(); }

        T& operator[](size_t index) const { return begin_[index]; }
    };

    template<typename T> requires(!std::is_const_v<T>)
    struct VectorView<T> : VectorView<const T>
    {
        using super = VectorView<const T>;
        using super::begin_;
        using super::end_;

        template<size_t N>
        VectorView(T(&arr)[N]) noexcept : super{arr} {}
        VectorView(T* begin, T* end) noexcept : super{ begin, end } {}

        VectorView() noexcept = default;

        VectorView(const VectorView&) noexcept = default;
        VectorView& operator = (const VectorView&) & noexcept = default;

        VectorView(VectorView&&) noexcept = default;
        VectorView& operator = (VectorView&&) & noexcept = default;

        ~VectorView() noexcept = default;

        VectorView(Vector<T>&&) = delete;
        VectorView& operator = (Vector<T>&&) = delete;
        VectorView(Vector<const T>&&) = delete;
        VectorView& operator = (Vector<const T>&&) = delete;

        T* begin() const { return const_cast<T*>(super::begin()); }
        T* end() const { return const_cast<T*>(super::end()); }

        T* data() const { return const_cast<T*>(super::data()); }

        using super::size;

        T& operator[](size_t index) const { return const_cast<T&>(super::operator[](index)); }
    };

    inline struct uninitialized_tag_t {} uninitialized_tag;
    inline struct initialized_tag_t {} initialized_tag;
    inline struct non_preserving_uninitialized_tag_t {} non_preserving_uninitialized_tag;
    inline struct non_preserving_initialized_tag_t {} non_preserving_initialized_tag;

    template<typename T>
    struct Vector : VectorView<T>
    {
        using super = VectorView<T>;
        using super::begin_;
        using super::end_;
        Vector(T* memory, size_t s) : super(memory, memory+s) {}

        Vector() noexcept = default;

        explicit Vector(size_t s, uninitialized_tag_t = uninitialized_tag) requires(!std::is_const_v<T>) : Vector(new T[s], s) {}
        explicit Vector(size_t s, T value, initialized_tag_t = initialized_tag) : Vector(s, uninitialized_tag)
        {
            std::fill(begin(), end(), value);
        }

        Vector(std::initializer_list<T> l) : Vector(std::size(l))
        {
            std::copy(std::begin(l), std::end(l), begin());
        }

        // non-copyable
        Vector(const Vector<T>&) = delete;
        Vector& operator = (const Vector<T>&) = delete;

        // non-copyable from parent classes
        Vector(const VectorView<const T>&) = delete;
        Vector& operator = (const VectorView<const T>&) = delete;
        Vector(const VectorView<std::remove_const_t<T>>&) = delete;
        Vector& operator = (const VectorView<std::remove_const_t<T>>&) = delete;

        Vector(Vector&& rhs) noexcept(noexcept(std::exchange(static_cast<VectorView<T>&>(rhs), {}))) :
            super{std::exchange(static_cast<VectorView<T>&>(rhs), {})} {}

        Vector& operator = (Vector&& rhs) & noexcept(noexcept(std::exchange(static_cast<VectorView<T>&>(rhs), {})))
        {
            super::operator =(std::exchange(static_cast<VectorView<T>&>(rhs), {}));
            return *this;
        }

        Vector(Vector<std::remove_const_t<T>>&& rhs) noexcept(noexcept(std::exchange(static_cast<VectorView<T>&>(rhs), {}))) requires(std::is_const_v<T>) :
            super{ std::exchange(static_cast<VectorView<std::remove_const_t<T>>&>(rhs), {}) } {}

        Vector& operator = (Vector<std::remove_const_t<T>>&& rhs) & noexcept(noexcept(std::exchange(static_cast<VectorView<T>&>(rhs), {}))) requires(std::is_const_v<T>)
        {
            super::operator =(std::exchange(static_cast<VectorView<T>&>(rhs), {}));
            return *this;
        }

        ~Vector() noexcept
        {
            delete[] begin();
        }

        using super::begin;
        using super::end;
        using super::data;
        using super::operator[];
        using super::size;

        // implement non-const preserving resize

        void resize(size_t s, non_preserving_uninitialized_tag_t) requires(!std::is_const_v<T>)
        {
            const size_t old_size = this->size();
            if (old_size != s) {
                Vector<T> new_vector{ s, uninitialized_tag };
                *this = std::move(new_vector);
            }
        }

        void resize(size_t s, T value, non_preserving_initialized_tag_t) requires(!std::is_const_v<T>)
        {
            this->resize(s, non_preserving_uninitialized_tag);
            std::fill(this->begin(), this->end(), value);
        }

        void resize(size_t s, uninitialized_tag_t = uninitialized_tag) requires(!std::is_const_v<T>)
        {
            const size_t old_size = this->size();
            if (old_size != s) {
                Vector<T> new_vector{ s, uninitialized_tag };
                std::copy_n(this->begin(), std::min(s, old_size), std::begin(new_vector));
                *this = std::move(new_vector);
            }
        }

        void resize(size_t s, T value, initialized_tag_t = initialized_tag) requires(!std::is_const_v<T>)
        {
            const size_t old_size = this->size();
            if (old_size != s) {
                Vector<T> new_vector{ s, uninitialized_tag };
                std::fill(std::copy_n(this->begin(), std::min(s, old_size), std::begin(new_vector)), std::end(new_vector), value);
                *this = std::move(new_vector);
            }
        }

    };
}
