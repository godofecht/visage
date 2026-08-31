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

#include <cstdarg>

namespace visage {
  constexpr float kPi = 3.14159265358979323846f;

  class String;
  void debugLogString(const char* file, unsigned int line, const String& log_message);
  void debugLogArgs(const char* file, unsigned int line, const char* format, va_list arg_list);

  template<typename T>
  void debugLog(const char* file, unsigned int line, T message, ...) {
    debugLogString(file, line, message);
  }

  template<>
  inline void debugLog(const char* file, unsigned int line, const char* format, ...) {
    va_list args;
    va_start(args, format);
    debugLogArgs(file, line, format, args);
    va_end(args);
  }

  void debugAssert(bool condition, const char* file, unsigned int line);
  void forceCrash();
}

/**
 * @def VISAGE_FORCE_CRASH()
 * @brief Forces the application to crash immediately. Useful for debugging critical errors.
 */
#define VISAGE_FORCE_CRASH() visage::forceCrash()

#ifndef NDEBUG
/**
 * @def VISAGE_LOG(log, ...)
 * @brief Logs a debug message, including the file and line number.
 * @param log The message or format string to log.
 * @param ... Optional arguments for the format string.
 */
#define VISAGE_LOG(log, ...) visage::debugLog(__FILE__, int(__LINE__), log, ##__VA_ARGS__)
/**
 * @def VISAGE_ASSERT(condition)
 * @brief Asserts that a condition is true. If the condition is false, the application will terminate.
 * @param condition The condition to check.
 */
#define VISAGE_ASSERT(condition) visage::debugAssert((condition), __FILE__, int(__LINE__))
#define no_except

namespace visage {
  /**
   * @class InstanceCounter
   * @brief A template class to count the number of instances of a given type.
   *        This is primarily used for leak checking in debug builds.
   * @tparam T The type to count instances of.
   */
  template<typename T>
  class InstanceCounter {
  public:
    static InstanceCounter& instance() {
      static InstanceCounter instance;
      return instance;
    }

    ~InstanceCounter() { VISAGE_ASSERT(count_ == 0); }

    void add() { count_++; }
    void remove() { count_--; }

  private:
    int count_ = 0;
  };
  /**
   * @class LeakChecker
   * @brief A helper class that uses InstanceCounter to detect memory leaks.
   *        An instance of this class is added to another class to track its lifetime.
   * @tparam T The type to check for leaks.
   */
  template<typename T>
  class LeakChecker {
  public:
    LeakChecker() { InstanceCounter<T>::instance().add(); }

    LeakChecker(const LeakChecker& other) { InstanceCounter<T>::instance().add(); }

    ~LeakChecker() { InstanceCounter<T>::instance().remove(); }
  };
}
/**
 * @def VISAGE_LEAK_CHECKER(className)
 * @brief A macro to add a LeakChecker to a class.
 *        This should be placed inside the class definition.
 * @param className The name of the class to check for leaks.
 */
#define VISAGE_LEAK_CHECKER(className)             \
  friend class visage::InstanceCounter<className>; \
  static const char* vaLeakCheckerName() {         \
    return #className;                             \
  }                                                \
  visage::LeakChecker<className> leakChecker;

#else
#define VISAGE_ASSERT(x) ((void)0)
#define VISAGE_LOG(x, ...) ((void)0)
#define VISAGE_LEAK_CHECKER(className)
#define no_except noexcept
#endif

#if VISAGE_WINDOWS
/**
 * @def VISAGE_STDCALL
 * @brief Defines the __stdcall calling convention for Windows platforms.
 */
#define VISAGE_STDCALL __stdcall
#else
#define VISAGE_STDCALL
#endif