
#include <gtest/gtest.h>
#include <type_traits>
#include <Containers2/containers2.hpp>
#include <ranges>

using namespace containers2;

// VectorView<const T> copy constructors:
// - can trivially assign from anything
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, const VectorView<const int>&>);
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, const VectorView<int>&>);
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, const Vector<const int>&>);
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, const Vector<int>&>);

// VectorView<mutable T> copy constructors
// - can trivially assign from VectorView<mutable T> and Vector<mutable T>
// - cannot assign from VectorView<const T> and Vector<const T> (const violation)
static_assert(!std::is_assignable_v<VectorView<int>&, const VectorView<const int>&>);
static_assert(std::is_trivially_assignable_v<VectorView<int>&, const VectorView<int>&>);
static_assert(!std::is_assignable_v<VectorView<int>&, const Vector<const int>&>);
static_assert(std::is_trivially_assignable_v<VectorView<int>&, const Vector<int>&>);

// Vector<T> copy constructors: cannot assign from anything
static_assert(!std::is_assignable_v<Vector<const int>&, const VectorView<const int>&>);
static_assert(!std::is_assignable_v<Vector<const int>&, const VectorView<int>&>);
static_assert(!std::is_assignable_v<Vector<const int>&, const Vector<const int>&>);
static_assert(!std::is_assignable_v<Vector<const int>&, const Vector<int>&>);
static_assert(!std::is_assignable_v<Vector<int>&, const VectorView<const int>&>);
static_assert(!std::is_assignable_v<Vector<int>&, const VectorView<int>&>);
static_assert(!std::is_assignable_v<Vector<int>&, const Vector<const int>&>);
static_assert(!std::is_assignable_v<Vector<int>&, const Vector<int>&>);

// VectorView<const T> move constructors
// - can trivially assign from VectorView<T>
// - cannot assign from Vector<T> (loses ownership)
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, VectorView<const int>&&>);
static_assert(std::is_trivially_assignable_v<VectorView<const int>&, VectorView<int>&&>);
static_assert(!std::is_assignable_v<VectorView<const int>&, Vector<const int>&&>);
static_assert(!std::is_assignable_v<VectorView<const int>&, Vector<int>&&>);

// VectorView<mutable T> move constructors
// - can trivially assign from VectorView<mutable T>
// - cannot assign from VectorView<const T> (const violation)
// - cannot assign from Vector<T> (loses ownership)
static_assert(!std::is_assignable_v<VectorView<int>&, VectorView<const int>&&>);
static_assert(std::is_trivially_assignable_v<VectorView<int>&, VectorView<int>&&>);
static_assert(!std::is_assignable_v<VectorView<int>&, Vector<const int>&&>);
static_assert(!std::is_assignable_v<VectorView<int>&, Vector<int>&&>);

// Vector<const T> move constructor
// - cannot assign from VectorView<T> (cannot take ownership)
static_assert(!std::is_assignable_v<Vector<const int>&, VectorView<const int>&&>);
static_assert(!std::is_assignable_v<Vector<const int>&, VectorView<int>&&>);
// - can assign (not necessarily trivially) from Vector<T> (takes ownership)
static_assert(std::is_assignable_v<Vector<const int>&, Vector<const int>&&>);
static_assert(std::is_assignable_v<Vector<const int>&, Vector<int>&&>);

// Vector<mutable T> move constructor
// - cannot assign from VectorView<T> (cannot take ownership)
static_assert(!std::is_assignable_v<Vector<int>&, VectorView<const int>&&>);
static_assert(!std::is_assignable_v<Vector<int>&, VectorView<int>&&>);
// - cannot assign from Vector<const T> (const violation)
static_assert(!std::is_assignable_v<Vector<int>&, Vector<const int>&&>);
// - can assign (not necessarily trivially) from Vector<mutable T> (takes ownership)
static_assert(std::is_assignable_v<Vector<int>&, Vector<int>&&>);

TEST(Containers2, VectorViewAccess) {
    int array[]{ 5, 7, 12 };   

    VectorView<int> mutable_view_with_mutable_data{ array }; // 1
    VectorView<const int> mutable_view_with_const_data{ array }; // 2
    const VectorView<int> const_view_with_mutable_data{ array }; // 3
    const VectorView<const int> const_view_with_const_data{ array }; // 4

    EXPECT_EQ(array[0], 5);
    ASSERT_EQ(mutable_view_with_mutable_data[0], 5);
    ASSERT_EQ(mutable_view_with_const_data[0], 5);
    ASSERT_EQ(const_view_with_mutable_data[0], 5);
    ASSERT_EQ(const_view_with_const_data[0], 5);

    EXPECT_EQ(array[1], 7);
    ASSERT_EQ(mutable_view_with_mutable_data[1], 7);
    ASSERT_EQ(mutable_view_with_const_data[1], 7);
    ASSERT_EQ(const_view_with_mutable_data[1], 7);
    ASSERT_EQ(const_view_with_const_data[1], 7);

    mutable_view_with_mutable_data[0] = 0;
    ASSERT_EQ(array[0], 0);
    ASSERT_EQ(mutable_view_with_mutable_data[0], 0);
    ASSERT_EQ(mutable_view_with_const_data[0], 0);
    ASSERT_EQ(const_view_with_mutable_data[0], 0);
    ASSERT_EQ(const_view_with_const_data[0], 0);

    EXPECT_EQ(const_view_with_mutable_data[1], 7);
    const_view_with_mutable_data[1] = 1;
    ASSERT_EQ(array[1], 1);
    ASSERT_EQ(mutable_view_with_mutable_data[1], 1);
    ASSERT_EQ(mutable_view_with_const_data[1], 1);
    ASSERT_EQ(const_view_with_mutable_data[1], 1);
    ASSERT_EQ(const_view_with_const_data[1], 1);
}

TEST(Containers2, VectorViewSize) {
    int array[]{ 5, 7, 12 };

    VectorView<int> mutable_view_with_mutable_data{ array }; // 1
    VectorView<const int> mutable_view_with_const_data{ array }; // 2
    const VectorView<int> const_view_with_mutable_data{ array }; // 3
    const VectorView<const int> const_view_with_const_data{ array }; // 4

    EXPECT_EQ(std::size(array), 3);

    ASSERT_EQ(mutable_view_with_mutable_data.size(), 3);
    ASSERT_EQ(mutable_view_with_const_data.size(), 3);
    ASSERT_EQ(const_view_with_mutable_data.size(), 3);
    ASSERT_EQ(const_view_with_const_data.size(), 3);
    ASSERT_EQ(std::size(mutable_view_with_mutable_data), 3);
    ASSERT_EQ(std::size(mutable_view_with_const_data), 3);
    ASSERT_EQ(std::size(const_view_with_mutable_data), 3);
    ASSERT_EQ(std::size(const_view_with_const_data), 3);
}

TEST(Containers2, VectorViewIterate) {
    int array[]{ 0, 1, 2, 3, 4 };

    VectorView<int> mutable_view_with_mutable_data{ array }; // 1
    VectorView<const int> mutable_view_with_const_data{ array }; // 2
    const VectorView<int> const_view_with_mutable_data{ array }; // 3
    const VectorView<const int> const_view_with_const_data{ array }; // 4

    int i = 0;
    for(auto&el: mutable_view_with_mutable_data)
    {
        ASSERT_EQ(el, i);
        ++i;
    }

    i = 0;
    for (auto& el : mutable_view_with_const_data)
    {
        ASSERT_EQ(el, i);
        ++i;
    }

    i = 0;
    for (auto& el : const_view_with_mutable_data)
    {
        ASSERT_EQ(el, i);
        ++i;
    }

    i = 0;
    for (auto& el : const_view_with_const_data)
    {
        ASSERT_EQ(el, i);
        ++i;
    }

}

TEST(Containers2, VectorVectorViewAccess) {
    Vector<size_t> vector_singleton{ 3 }; // potentially confusing: with other types the same constructor builds an uninitialized Vector with 3 elements
    ASSERT_EQ(vector_singleton.size(), 1);

    Vector<size_t> vector_uninitialized{ 257, uninitialized_tag };
    ASSERT_EQ(vector_uninitialized.size(), 257);

    Vector<size_t> vector_initialized{ 257, 1, initialized_tag };
    ASSERT_EQ(vector_initialized.size(), 257);
    ASSERT_EQ(vector_initialized[0], 1);
    ASSERT_EQ(vector_initialized[128], 1);
    ASSERT_EQ(vector_initialized[256], 1);

    Vector<size_t> vector_2el{ 3, 2 };
    ASSERT_EQ(vector_2el.size(), 2);

    Vector<int> vector{5, 7, 12 };

    VectorView<int> mutable_view_with_mutable_data{ vector }; // 1
    VectorView<const int> mutable_view_with_const_data{ vector }; // 2
    const VectorView<int> const_view_with_mutable_data{ vector }; // 3
    const VectorView<const int> const_view_with_const_data{ vector }; // 4

    EXPECT_EQ(vector[0], 5);
    ASSERT_EQ(mutable_view_with_mutable_data[0], 5);
    ASSERT_EQ(mutable_view_with_const_data[0], 5);
    ASSERT_EQ(const_view_with_mutable_data[0], 5);
    ASSERT_EQ(const_view_with_const_data[0], 5);

    EXPECT_EQ(vector[1], 7);
    ASSERT_EQ(mutable_view_with_mutable_data[1], 7);
    ASSERT_EQ(mutable_view_with_const_data[1], 7);
    ASSERT_EQ(const_view_with_mutable_data[1], 7);
    ASSERT_EQ(const_view_with_const_data[1], 7);

    mutable_view_with_mutable_data[0] = 0;
    ASSERT_EQ(vector[0], 0);
    ASSERT_EQ(mutable_view_with_mutable_data[0], 0);
    ASSERT_EQ(mutable_view_with_const_data[0], 0);
    ASSERT_EQ(const_view_with_mutable_data[0], 0);
    ASSERT_EQ(const_view_with_const_data[0], 0);

    EXPECT_EQ(const_view_with_mutable_data[1], 7);
    const_view_with_mutable_data[1] = 1;
    ASSERT_EQ(vector[1], 1);
    ASSERT_EQ(mutable_view_with_mutable_data[1], 1);
    ASSERT_EQ(mutable_view_with_const_data[1], 1);
    ASSERT_EQ(const_view_with_mutable_data[1], 1);
    ASSERT_EQ(const_view_with_const_data[1], 1);
}

TEST(Containers2, VectorVectorCopy) {
    const Vector<int> vector_source{ 5, 7, 12 };
    Vector<int> vector_target{};
    vector_target.resize(vector_source.size(), non_preserving_uninitialized_tag);
    std::copy(std::begin(vector_source), std::end(vector_source), std::begin(vector_target));
    ASSERT_EQ(vector_source[0], vector_target[0]);
    ASSERT_EQ(vector_source[1], vector_target[1]);
    ASSERT_EQ(vector_source[2], vector_target[2]);
}

TEST(Containers2, VectorResize) {
    Vector<int> vector{ 5, 7, 12 };
    vector.resize(5, 24); // grow initialized
    ASSERT_EQ(vector.size(), 5);
    ASSERT_EQ(vector[0], 5);
    ASSERT_EQ(vector[1], 7);
    ASSERT_EQ(vector[2], 12);
    ASSERT_EQ(vector[3], 24);
    ASSERT_EQ(vector[4], 24);
    vector.resize(6); // grow uninitialized
    ASSERT_EQ(vector.size(), 6);
    ASSERT_EQ(vector[0], 5);
    ASSERT_EQ(vector[1], 7);
    ASSERT_EQ(vector[2], 12);
    ASSERT_EQ(vector[3], 24);
    ASSERT_EQ(vector[4], 24);
    vector.resize(4); // shrink uninitialized
    ASSERT_EQ(vector.size(), 4);
    ASSERT_EQ(vector[0], 5);
    ASSERT_EQ(vector[1], 7);
    ASSERT_EQ(vector[2], 12);
    ASSERT_EQ(vector[3], 24);
    vector.resize(2, 127); // shrink initialized
    ASSERT_EQ(vector.size(), 2);
    ASSERT_EQ(vector[0], 5);
    ASSERT_EQ(vector[1], 7);
    vector.resize(4, 87, non_preserving_initialized_tag); // resize non preserving initialized
    ASSERT_EQ(vector.size(), 4);
    ASSERT_EQ(vector[0], 87);
    ASSERT_EQ(vector[1], 87);
    ASSERT_EQ(vector[2], 87);
    ASSERT_EQ(vector[3], 87);
    vector.resize(76, non_preserving_uninitialized_tag); // resize non preserving uninitialized
    ASSERT_EQ(vector.size(), 76);
}

TEST(Containers2, VectorUninitializedConstruction) {
    const Vector<int> vector_uninitialized{ 42, uninitialized_tag };
    ASSERT_EQ(vector_uninitialized.size(), 42);
}

TEST(Containers2, VectorInitializedConstruction) {
    const Vector<int> vector_initialized{ 2, 42, initialized_tag };
    ASSERT_EQ(vector_initialized[0], 42);
    ASSERT_EQ(vector_initialized[1], 42);
}

TEST(Containers2, VectorMoveConstruction) {
    Vector<int> vector_moved_from{ 2, 42, initialized_tag };
    Vector<int> vector_moved_to_mutable{ std::move(vector_moved_from) };
    ASSERT_EQ(vector_moved_to_mutable[0], 42);
    ASSERT_EQ(vector_moved_to_mutable[1], 42);
    Vector<const int> vector_moved_to_const{ std::move(vector_moved_to_mutable) };
    ASSERT_EQ(vector_moved_to_const[0], 42);
    ASSERT_EQ(vector_moved_to_const[1], 42);
}

TEST(Containers2, VectorMoveAssignment) {
    Vector<int> vector_moved_from{ 2, 42, initialized_tag };
    Vector<int> vector_moved_to_mutable{};
    vector_moved_to_mutable = std::move(vector_moved_from);
    ASSERT_EQ(vector_moved_to_mutable[0], 42);
    ASSERT_EQ(vector_moved_to_mutable[1], 42);
    Vector<const int> vector_moved_to_const{};
    vector_moved_to_const = std::move(vector_moved_to_mutable);
    ASSERT_EQ(vector_moved_to_const[0], 42);
    ASSERT_EQ(vector_moved_to_const[1], 42);
}

TEST(Containers2, VectorViewCopyConstruction) {
    int array[]{ 5, 7, 12 };

    VectorView<int> mutable_view_with_mutable_data{ array }; // 1
    VectorView<const int> mutable_view_with_const_data{ array }; // 2
    const VectorView<int> const_view_with_mutable_data{ array }; // 3
    const VectorView<const int> const_view_with_const_data{ array }; // 4

    VectorView<int> copy_of_mutable_view_with_mutable_data_1{ mutable_view_with_mutable_data }; // 1
    VectorView<const int> copy_of_mutable_view_with_const_data_1{ mutable_view_with_const_data }; // 2
    const VectorView<int> copy_of_const_view_with_mutable_data_1{ const_view_with_mutable_data }; // 3
    const VectorView<const int> copy_of_const_view_with_const_data_1{ const_view_with_const_data }; // 4

    VectorView<int> copy_of_mutable_view_with_mutable_data_2{ }; // 1
    VectorView<const int> copy_of_mutable_view_with_const_data_2{ }; // 2
    VectorView<int> copy_of_const_view_with_mutable_data_2{ }; // 3
    VectorView<const int> copy_of_const_view_with_const_data_2{ }; // 4
    copy_of_mutable_view_with_mutable_data_2 = mutable_view_with_mutable_data; // 1
    copy_of_mutable_view_with_const_data_2 = mutable_view_with_const_data; // 2
    copy_of_const_view_with_mutable_data_2 = const_view_with_mutable_data; // 3
    copy_of_const_view_with_const_data_2 = const_view_with_const_data; // 4
}
