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

#include "embedded/example_fonts.h"

#include <visage/app.h>
#include <visage_widgets/button.h>

using namespace visage::dimension;

// This example demonstrates how to create and manage multiple windows.

// A custom class for the secondary window. It inherits from ApplicationWindow,
// just like the main window.
class SubWindow : public visage::ApplicationWindow {
public:
  void draw(visage::Canvas& canvas) override {
    // Draw a simple background and a circle.
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());

    canvas.setColor(0xffaa88ff);
    int radius = std::min(width(), height()) / 4;
    canvas.circle(width() / 2 - radius, height() / 2 - radius, 2 * radius);
  }
};

// The main application window.
class ExampleEditor : public visage::ApplicationWindow {
public:
  static constexpr int kNumFrames = 10;

  ExampleEditor() {
    setFlexLayout(true);
    layout().setPadding(50_px);

    // Add a button that will be used to open and close the sub-window.
    addChild(&button_);
    button_.setText("Open Window");
    button_.layout().setHeight(100_vh);
    button_.layout().setWidth(100_vw);
    button_.setFont(visage::Font(24, resources::fonts::Lato_Regular_ttf));

    // --- Window Event Handling ---

    // The sub-window is a member of the main editor. We can subscribe to its events.
    // Here, we update the button text when the sub-window is shown or hidden.
    sub_window_.onShow() += [this] { button_.setText("Close Window"); };
    sub_window_.onHide() += [this] { button_.setText("Open Window"); };

    // Handle the button click to toggle the sub-window's visibility.
    button_.onToggle() += [this](visage::Button* button, bool is_toggled) {
      if (sub_window_.isShowing())
        sub_window_.hide();
      else
        // The `show` method for a window is overloaded to accept position and size.
        sub_window_.show(10_vw, 10_vh, 400_px, 300_px);
    };

    // It's good practice to also hide child windows when the main window is hidden.
    onHide() += [this] {
      if (sub_window_.isShowing())
        sub_window_.hide();
    };
  }

  void draw(visage::Canvas& canvas) override {
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());
  }

private:
  visage::UiButton button_;
  visage::Font font_ {};
  SubWindow sub_window_;
};

int runExample() {
  ExampleEditor editor;
  editor.show(500_px, 200_px);
  editor.runEventLoop();

  return 0;
}
