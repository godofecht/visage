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

#include "clap_plugin.h"

#include <clap/helpers/host-proxy.hh>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>

using namespace visage::dimension;

// Define the features of this plugin. This one is an "instrument".
static const char* kClapFeatures[] = { CLAP_PLUGIN_FEATURE_INSTRUMENT, nullptr };

// Initialize the static plugin descriptor.
clap_plugin_descriptor ClapPlugin::descriptor = { CLAP_VERSION,          "dev.visage.example",
                                                  "Example Clap Plugin", "Visage",
                                                  "visage.dev",          "visage.dev",
                                                  "visage.dev",          "0.0.1",
                                                  "Example Clap Plugin", kClapFeatures };

ClapPlugin::ClapPlugin(const clap_host* host) : ClapPluginBase(&descriptor, host) { }

ClapPlugin::~ClapPlugin() = default;

#ifdef __linux__
// On Linux, the host may ask the plugin to register its file descriptor
// to be managed by the host's event loop.
void ClapPlugin::onPosixFd(int fd, clap_posix_fd_flags_t flags) noexcept {
  if (app_ && app_->window())
    // `processPluginFdEvents` tells the Visage window to handle any pending events.
    app_->window()->processPluginFdEvents();
}
#endif

// Check for the supported native windowing API (e.g., Win32, Cocoa, X11).
bool ClapPlugin::guiIsApiSupported(const char* api, bool is_floating) noexcept {
  if (is_floating)
    return false;

#ifdef _WIN32
  if (strcmp(api, CLAP_WINDOW_API_WIN32) == 0)
    return true;
#elif __APPLE__
  if (strcmp(api, CLAP_WINDOW_API_COCOA) == 0)
    return true;
#elif __linux__
  if (strcmp(api, CLAP_WINDOW_API_X11) == 0)
    return true;
#endif

  return false;
}

// This is where the Visage UI is created and configured.
bool ClapPlugin::guiCreate(const char* api, bool is_floating) noexcept {
  if (is_floating)
    return false;

  if (app_)
    return true;

  // Create an ApplicationWindow, which will manage the drawing and event handling.
  app_ = std::make_unique<visage::ApplicationWindow>();
  app_->setWindowDimensions(80_vmin, 60_vmin);

  // Set up a simple draw callback, just like in the standalone examples.
  app_->onDraw() = [this](visage::Canvas& canvas) {
    canvas.setColor(0xff000066);
    canvas.fill(0, 0, app_->width(), app_->height());

    float circle_radius = app_->height() * 0.1f;
    float x = app_->width() * 0.5f - circle_radius;
    float y = app_->height() * 0.5f - circle_radius;
    canvas.setColor(0xff00ffff);
    canvas.circle(x, y, 2.0f * circle_radius);
  };

  // When the plugin's content is resized (e.g., by the user),
  // we need to notify the host so it can adjust the parent window.
  app_->onWindowContentsResized() = [this] { _host.guiRequestResize(pluginWidth(), pluginHeight()); };

  return true;
}

// Clean up the Visage UI.
void ClapPlugin::guiDestroy() noexcept {
#if __linux__
  // Unregister the file descriptor on Linux.
  if (app_ && app_->window() && _host.canUsePosixFdSupport())
    _host.posixFdSupportUnregister(app_->window()->posixFd());
#endif

  app_->close();
}

// The host provides a native window handle. Visage's `show` method can accept this
// handle and embed the Visage content within the host's window.
bool ClapPlugin::guiSetParent(const clap_window* window) noexcept {
  if (app_ == nullptr)
    return false;

  app_->show(window->ptr);

#if __linux__
  // Register the file descriptor on Linux.
  if (_host.canUsePosixFdSupport() && app_->window()) {
    int fd_flags = CLAP_POSIX_FD_READ | CLAP_POSIX_FD_WRITE | CLAP_POSIX_FD_ERROR;
    return _host.posixFdSupportRegister(app_->window()->posixFd(), fd_flags);
  }
#endif
  return true;
}

bool ClapPlugin::guiGetResizeHints(clap_gui_resize_hints_t* hints) noexcept {
  if (app_ == nullptr)
    return false;

  bool fixed_aspect_ratio = app_->isFixedAspectRatio();
  hints->can_resize_horizontally = true;
  hints->can_resize_vertically = true;
  hints->preserve_aspect_ratio = fixed_aspect_ratio;

  if (fixed_aspect_ratio) {
    hints->aspect_ratio_width = app_->height() * app_->aspectRatio();
    hints->aspect_ratio_height = app_->width();
  }
  return true;
}

bool ClapPlugin::guiAdjustSize(uint32_t* width, uint32_t* height) noexcept {
  if (app_ == nullptr)
    return false;

  app_->adjustWindowDimensions(width, height, true, true);
  return true;
}

bool ClapPlugin::guiSetSize(uint32_t width, uint32_t height) noexcept {
  if (app_ == nullptr)
    return false;

  setPluginDimensions(width, height);
  return true;
}

bool ClapPlugin::guiGetSize(uint32_t* width, uint32_t* height) noexcept {
  if (app_ == nullptr)
    return false;

  *width = pluginWidth();
  *height = pluginHeight();
  return true;
}
