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

#include "visage_ui/frame.h"

namespace visage {
  class ApplicationEditor;
  class Canvas;
  class Window;
  class WindowEventHandler;
  class ClientWindowDecoration;
  /**
   * @class TopLevelFrame
   * @brief Represents the top-level frame in the application editor.
   *
   * This class is a specialized Frame that serves as the root of the UI hierarchy.
   * It manages client-side window decorations and resizing.
   */
  class TopLevelFrame : public Frame {
  public:
    /**
     * @brief Constructs a TopLevelFrame.
     * @param editor A pointer to the ApplicationEditor that owns this frame.
     */
    explicit TopLevelFrame(ApplicationEditor* editor);
    /**
     * @brief Destroys the TopLevelFrame.
     */
    ~TopLevelFrame() override;

    /**
     * @brief Called when the frame is resized.
     */
    void resized() override;
    /**
     * @brief Adds client-side window decorations to the frame.
     */
    void addClientDecoration();
    /**
     * @brief Checks if the frame has client-side window decorations.
     * @return true if the frame has client decorations, false otherwise.
     */
    bool hasClientDecoration() const { return client_decoration_ != nullptr; }

  private:
    ApplicationEditor* editor_ = nullptr;
    std::unique_ptr<ClientWindowDecoration> client_decoration_;
  };

  /**
   * @class ApplicationEditor
   * @brief Manages the main application view and its interaction with the window.
   *
   * This class is responsible for drawing the UI, handling events, and managing
   * the window it is attached to.
   */
  class ApplicationEditor : public Frame {
  public:
    /**
     * @brief Default height for the client-side title bar.
     */
    static constexpr int kDefaultClientTitleBarHeight = 30;

    /**
     * @brief Constructs an ApplicationEditor.
     */
    ApplicationEditor();
    /**
     * @brief Destroys the ApplicationEditor.
     */
    ~ApplicationEditor() override;

    /**
     * @brief Takes a screenshot of the current view.
     * @return A const reference to the Screenshot object.
     */
    const Screenshot& takeScreenshot();

    /**
     * @brief Sets the canvas details for drawing.
     */
    void setCanvasDetails();

    /**
     * @brief Adds the editor to a window.
     * @param window A pointer to the Window to attach to.
     */
    void addToWindow(Window* window);
    /**
     * @brief Sets the editor to be windowless with specified dimensions.
     * @param width The width of the windowless editor.
     * @param height The height of the windowless editor.
     */
    void setWindowless(int width, int height);
    /**
     * @brief Removes the editor from the window.
     */
    void removeFromWindow();
    /**
     * @brief Draws the window contents.
     */
    void drawWindow();

    /**
     * @brief Checks if the aspect ratio is fixed.
     * @return true if the aspect ratio is fixed, false otherwise.
     */
    bool isFixedAspectRatio() const { return fixed_aspect_ratio_ != 0.0f; }
    /**
     * @brief Sets whether the aspect ratio should be fixed.
     * @param fixed true to fix the aspect ratio, false otherwise.
     */
    void setFixedAspectRatio(bool fixed);
    /**
     * @brief Gets the aspect ratio of the editor.
     * @return The aspect ratio.
     */
    float aspectRatio() const override {
      if (height() && width())
        return width() * 1.0f / height();
      return 1.0f;
    }

    /**
     * @brief Gets the window the editor is attached to.
     * @return A pointer to the Window.
     */
    Window* window() const { return window_; }

    /**
     * @brief Draws stale children of the frame.
     */
    void drawStaleChildren();

    /**
     * @brief Sets the minimum dimensions for the editor.
     * @param width The minimum width.
     * @param height The minimum height.
     */
    void setMinimumDimensions(float width, float height) {
      min_width_ = std::max(0.0f, width);
      min_height_ = std::max(0.0f, height);
    }

    /**
     * @brief Checks and updates the fixed aspect ratio if necessary.
     */
    void checkFixedAspectRatio() {
      if (fixed_aspect_ratio_ && width() && height())
        fixed_aspect_ratio_ = aspectRatio();
    }

    /**
     * @brief Adjusts window dimensions based on constraints.
     * @param width A pointer to the width to adjust.
     * @param height A pointer to the height to adjust.
     * @param horizontal_resize Whether horizontal resizing is allowed.
     * @param vertical_resize Whether vertical resizing is allowed.
     */
    void adjustWindowDimensions(int* width, int* height, bool horizontal_resize, bool vertical_resize) const;

    /**
     * @brief Adjusts window dimensions based on constraints.
     * @param width A pointer to the width to adjust.
     * @param height A pointer to the height to adjust.
     * @param horizontal_resize Whether horizontal resizing is allowed.
     * @param vertical_resize Whether vertical resizing is allowed.
     */
    void adjustWindowDimensions(uint32_t* width, uint32_t* height, bool horizontal_resize,
                                bool vertical_resize) const {
      int w = *width;
      int h = *height;
      adjustWindowDimensions(&w, &h, horizontal_resize, vertical_resize);
      *width = w;
      *height = h;
    }

    /**
     * @brief Adds client-side window decorations.
     */
    void addClientDecoration() { top_level_->addClientDecoration(); }
    /**
     * @brief Performs a hit test at the given position.
     * @param position The position to test.
     * @return The HitTestResult.
     */
    HitTestResult hitTest(const Point& position) const override {
      if (position.y < kDefaultClientTitleBarHeight && top_level_->hasClientDecoration())
        return HitTestResult::TitleBar;

      return HitTestResult::Client;
    }

  private:
    Window* window_ = nullptr;
    FrameEventHandler event_handler_;
    std::unique_ptr<Canvas> canvas_;
    std::unique_ptr<TopLevelFrame> top_level_;
    std::unique_ptr<WindowEventHandler> window_event_handler_;
    float fixed_aspect_ratio_ = 0.0f;

    float min_width_ = 0.0f;
    float min_height_ = 0.0f;
    std::vector<Frame*> stale_children_;
    std::vector<Frame*> drawing_children_;

    VISAGE_LEAK_CHECKER(ApplicationEditor)
  };
}
