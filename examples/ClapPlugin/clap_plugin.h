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

#include <clap/helpers/plugin.hh>
#include <visage/app.h>

// This example demonstrates how to use Visage to create a GUI for a CLAP audio plugin.
// The `clap-helpers` library is used to simplify the boilerplate of CLAP plugin creation.

// Define a base class for the plugin using the clap::helpers template.
using ClapPluginBase = clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                             clap::helpers::CheckingLevel::Maximal>;

// The main plugin class. It inherits from the CLAP helper base class.
class ClapPlugin : public ClapPluginBase {
public:
  // The static descriptor that tells the host about this plugin.
  static clap_plugin_descriptor descriptor;

  explicit ClapPlugin(const clap_host* host);
  ~ClapPlugin() override;

#ifdef __linux__
  // --- POSIX FD Support (Linux specific) ---
  // This allows the host to integrate the plugin's event loop with its own.
  bool implementsPosixFdSupport() const noexcept override { return true; }
  void onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept override;
#endif

protected:
  // --- CLAP GUI Extension Implementation ---
  // These methods are overrides from the clap::helpers::Plugin base class
  // and correspond to the functions in the `clap_plugin_gui` extension.

  // Does this plugin have a GUI?
  bool implementsGui() const noexcept override { return true; }
  // Check if the host's windowing API is supported.
  bool guiIsApiSupported(const char* api, bool is_floating) noexcept override;
  // Create the GUI.
  bool guiCreate(const char* api, bool is_floating) noexcept override;
  // Destroy the GUI.
  void guiDestroy() noexcept override;
  // Attach the plugin's GUI to a parent window provided by the host.
  bool guiSetParent(const clap_window* window) noexcept override;
  bool guiSetScale(double scale) noexcept override { return false; }
  // Can the GUI be resized by the user?
  bool guiCanResize() const noexcept override { return true; }
  // Provide resizing hints to the host.
  bool guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept override;
  // Allow the host to suggest a new size.
  bool guiAdjustSize(uint32_t* width, uint32_t* height) noexcept override;
  // The host is setting the size of the GUI.
  bool guiSetSize(uint32_t width, uint32_t* height) noexcept override;
  // The host is requesting the size of the GUI.
  bool guiGetSize(uint32_t* width, uint32_t* height) noexcept override;

private:
  int pluginWidth() const {
    if (app_ == nullptr)
      return 0;

#if __APPLE__
    return app_->width();
#else
    return app_->nativeWidth();
#endif
  }

  int pluginHeight() const {
    if (app_ == nullptr)
      return 0;

#if __APPLE__
    return app_->height();
#else
    return app_->nativeHeight();
#endif
  }

  void setPluginDimensions(int width, int height) {
    if (app_ == nullptr)
      return;
#if __APPLE__
    app_->setWindowDimensions(width, height);
#else
    app_->setNativeWindowDimensions(width, height);
#endif
  }

  std::unique_ptr<visage::ApplicationWindow> app_;
};
