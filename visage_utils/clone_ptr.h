/* Copyright Vital Audio, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <memory>

namespace visage {
  /**
   * @class clone_ptr
   * @brief A smart pointer that owns and manages another object through a pointer
   *        and disposes of that object when the clone_ptr goes out of scope.
   *        It behaves like std::unique_ptr but provides a copy constructor that
   *        creates a deep copy of the managed object.
   * @tparam T The type of the managed object.
   */
  template<class T>
  class clone_ptr {
  public:
    static_assert(std::is_copy_constructible_v<T>, "T must be copy constructible for clone_ptr");

    clone_ptr() = default;
    clone_ptr(std::unique_ptr<T> p) : p_(std::move(p)) { }

    clone_ptr(const clone_ptr& other) : p_(other.p_ ? std::make_unique<T>(*other.p_) : nullptr) { }

    clone_ptr& operator=(const clone_ptr& other) {
      if (this != &other)
        p_ = other.p_ ? std::make_unique<T>(*other.p_) : nullptr;
      return *this;
    }

    clone_ptr(clone_ptr&&) noexcept = default;
    clone_ptr& operator=(clone_ptr&&) noexcept = default;
    /**
     * @brief Resets the clone_ptr, deleting the managed object.
     */
    void reset() { p_.reset(); }
    /**
     * @brief Resets the clone_ptr to manage a new object.
     * @param p A unique_ptr to the new object.
     */
    void reset(std::unique_ptr<T> p) { p_ = std::move(p); }
    /**
     * @brief Swaps the managed object with another clone_ptr.
     * @param other The other clone_ptr to swap with.
     */
    void swap(clone_ptr& other) noexcept { p_.swap(other.p_); }
    /**
     * @brief Gets a pointer to the managed object.
     * @return A pointer to the managed object, or nullptr if the clone_ptr is empty.
     */
    T* get() const { return p_.get(); }
    /**
     * @brief Dereferences the pointer to the managed object.
     * @return A reference to the managed object.
     */
    T& operator*() const { return p_.operator*(); }
    /**
     * @brief Dereferences the pointer to the managed object.
     * @return A pointer to the managed object.
     */
    T* operator->() const { return p_.operator->(); }
    explicit operator bool() const { return static_cast<bool>(p_); }

  private:
    std::unique_ptr<T> p_;
  };
}
