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

#include "application_editor.h"

#include <visage_windowing/windowing.h>

namespace visage {
  /**
   * @brief Represents the main application window.
   *
   * This class provides functionality for creating, showing, and managing the
   * application window. It inherits from ApplicationEditor, which provides the
   * drawing and event handling capabilities.
   */
  class ApplicationWindow : public ApplicationEditor {
  public:
    /**
     * @brief Constructs an ApplicationWindow object.
     */
    ApplicationWindow();
    /**
     * @brief Destroys the ApplicationWindow object.
     */
    ~ApplicationWindow() override;

    /**
     * @brief Returns the title of the window.
     * @return The current window title.
     */
    const std::string& title() const { return title_; }
    /**
     * @brief Sets the title of the window.
     * @param title The new title for the window.
     */
    void setTitle(std::string title);

    /**
     * @brief Checks if the window is set to be always on top.
     * @return True if the window is always on top, false otherwise.
     */
    bool isAlwaysOnTop() const { return always_on_top_; }
    /**
     * @brief Sets the window to be always on top.
     * @param on_top True to set the window always on top, false otherwise.
     */
    void setWindowOnTop(bool on_top);

    /**
     * @brief Sets the window decoration type.
     * @param decoration The type of window decoration to use.
     */
    void setWindowDecoration(Window::Decoration decoration) {
      decoration_ = decoration;
      if (decoration_ == Window::Decoration::Client)
        addClientDecoration();
    }

    /**
     * @brief Sets the native dimensions of the window.
     * @param width The width of the window in pixels.
     * @param height The height of the window in pixels.
     */
    void setNativeWindowDimensions(int width, int height) {
      setNativeBounds(nativeX(), nativeY(), width, height);
      if (window_)
        window_->setNativeWindowSize(width, height);
    }

    /**
     * @brief Sets the dimensions of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     */
    void setWindowDimensions(const Dimension& width, const Dimension& height);
    /**
     * @brief Sets the position and dimensions of the window.
     * @param x The x-coordinate of the window.
     * @param y The y-coordinate of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     */
    void setWindowDimensions(const Dimension& x, const Dimension& y, const Dimension& width,
                             const Dimension& height);

    /**
     * @brief Shows the window.
     */
    void show();
    /**
     * @brief Shows the window with a parent window.
     * @param parent_window A pointer to the parent window.
     */
    void show(void* parent_window);
    /**
     * @brief Shows the window with specified dimensions and a parent window.
     * @param width The width of the window.
     * @param height The height of the window.
     * @param parent_window A pointer to the parent window.
     */
    void show(const Dimension& width, const Dimension& height, void* parent_window);
    /**
     * @brief Shows the window with specified dimensions.
     * @param width The width of the window.
     * @param height The height of the window.
     */
    void show(const Dimension& width, const Dimension& height);
    /**
     * @brief Shows the window with specified position and dimensions.
     * @param x The x-coordinate of the window.
     * @param y The y-coordinate of the window.
     * @param width The width of the window.
     * @param height The height of the window.
     */
    void show(const Dimension& x, const Dimension& y, const Dimension& width, const Dimension& height);
    /**
     * @brief Shows the window maximized.
     */
    void showMaximized();
    /**
     * @brief Hides the window.
     */
    void hide();
    /**
     * @brief Closes the window.
     */
    void close();
    /**
     * @brief Checks if the window is currently showing.
     * @return True if the window is showing, false otherwise.
     */
    bool isShowing() const;

    /**
     * @brief Returns a callback list for the window show event.
     * @return A reference to the on_show_ callback list.
     */
    auto& onShow() { return on_show_; }
    /**
     * @brief Returns a callback list for the window hide event.
     * @return A reference to the on_hide_ callback list.
     */
    auto& onHide() { return on_hide_; }
    /**
     * @brief Returns a callback list for the window contents resized event.
     * @return A reference to the on_window_contents_resized_ callback list.
     */
    auto& onWindowContentsResized() { return on_window_contents_resized_; }

    /**
     * @brief Runs the event loop for the window.
     */
    void runEventLoop();

  private:
    void registerCallbacks();
    void showWindow(bool maximized);

    IPoint initial_position_;
    Window::Decoration decoration_ = Window::Decoration::Native;
    CallbackList<void()> on_show_;
    CallbackList<void()> on_hide_;
    CallbackList<void()> on_window_contents_resized_;
    std::string title_;
    bool always_on_top_ = false;
    std::unique_ptr<Window> window_;
  };
}
