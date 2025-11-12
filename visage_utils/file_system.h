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

#include <filesystem>
#include <vector>

namespace visage {
  typedef std::filesystem::path File;

  /**
   * @brief Replaces the content of a file with the given binary data.
   * @param file The file to replace.
   * @param data A pointer to the data.
   * @param size The size of the data in bytes.
   * @return True if the file was replaced successfully, false otherwise.
   */
  bool replaceFileWithData(const File& file, const unsigned char* data, size_t size);
  /**
   * @brief Replaces the content of a file with the given text.
   * @param file The file to replace.
   * @param text The new text content.
   * @return True if the file was replaced successfully, false otherwise.
   */
  bool replaceFileWithText(const File& file, const std::string& text);
  /**
   * @brief Checks if the application has write access to a file or directory.
   * @param file The file or directory to check.
   * @return True if write access is granted, false otherwise.
   */
  bool hasWriteAccess(const File& file);
  /**
   * @brief Checks if a file exists.
   * @param file The file to check.
   * @return True if the file exists, false otherwise.
   */
  bool fileExists(const File& file);
  /**
   * @brief Appends text to the end of a file.
   * @param file The file to append to.
   * @param text The text to append.
   * @return True if the text was appended successfully, false otherwise.
   */
  bool appendTextToFile(const File& file, const std::string& text);
  /**
   * @brief Loads the entire content of a file into a binary buffer.
   * @param file The file to load.
   * @param[out] size The size of the loaded data in bytes.
   * @return A unique_ptr to the buffer containing the file data, or nullptr on failure.
   */
  std::unique_ptr<unsigned char[]> loadFileData(const File& file, size_t& size);
  /**
   * @brief Loads the entire content of a file as a string.
   * @param file The file to load.
   * @return A string containing the file content.
   */
  std::string loadFileAsString(const File& file);
  /**
   * @brief Gets the path to the current host executable.
   * @return A File object representing the path to the executable.
   */
  File hostExecutable();
  /**
   * @brief Gets the path to the application data directory.
   * @return A File object representing the path to the app data directory.
   */
  File appDataDirectory();
  /**
   * @brief Gets the path to the user's documents directory.
   * @return A File object representing the path to the documents directory.
   */
  File userDocumentsDirectory();
  /**
   * @brief Creates a temporary file with a unique name.
   * @param extension The file extension to use for the temporary file.
   * @return A File object representing the path to the newly created temporary file.
   */
  File createTemporaryFile(const std::string& extension);
  /**
   * @brief Gets the name of a file from a path, including the extension.
   * @param file The file path.
   * @return The file name as a string.
   */
  std::string fileName(const File& file);
  /**
   * @brief Gets the stem of a file from a path (the name without the extension).
   * @param file The file path.
   * @return The file stem as a string.
   */
  std::string fileStem(const File& file);
  /**
   * @brief Gets the host name of the computer.
   * @return The host name as a string.
   */
  std::string hostName();
  /**
   * @brief Searches for files in a directory that match a regular expression.
   * @param directory The directory to search in.
   * @param regex The regular expression to match against file names.
   * @return A vector of File objects for the matching files.
   */
  std::vector<File> searchForFiles(const File& directory, const std::string& regex);
  /**
   * @brief Searches for directories in a directory that match a regular expression.
   * @param directory The directory to search in.
   * @param regex The regular expression to match against directory names.
   * @return A vector of File objects for the matching directories.
   */
  std::vector<File> searchForDirectories(const File& directory, const std::string& regex);
}