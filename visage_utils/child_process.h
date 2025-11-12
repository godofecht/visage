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

#include <string>

namespace visage {
  /**
   * @brief The default timeout in milliseconds for a child process to complete.
   */
  static constexpr int kDefaultChildProcessTimeoutMs = 10000;
  /**
   * @brief The maximum size of the output that will be read from the child process.
   */
  static constexpr size_t kMaxOutputSize = 1024 * 1024;
  /**
   * @brief Spawns a child process and waits for it to complete.
   *
   * This function executes a command in a new process and captures its standard output.
   *
   * @param command The command to execute.
   * @param arguments The arguments to pass to the command.
   * @param[out] output A string to store the standard output of the process.
   * @param timeout_ms The maximum time in milliseconds to wait for the process to complete.
   * @return True if the process was spawned and completed successfully within the timeout, false otherwise.
   */
  bool spawnChildProcess(const std::string& command, const std::string& arguments,
                         std::string& output, int timeout_ms = kDefaultChildProcessTimeoutMs);
}
