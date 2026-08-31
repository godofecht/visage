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

#include <visage/app.h>

using namespace visage::dimension;

// This example demonstrates the use of Visage's flexbox-based layout system.
class ExampleEditor : public visage::ApplicationWindow {
public:
  static constexpr int kNumFrames = 10;

  ExampleEditor() {
    // Enable flexbox layout for the main application window.
    setFlexLayout(true);

    // Set various flexbox properties.
    // Dimensions can be specified in different units, like logical pixels (_px)
    // or viewport-relative units (_vmin, _vmax, _vw, _vh).
    layout().setPadding(10_px); // Padding around the container.
    layout().setFlexGap(10_px);   // Gap between child items.

    // Flexbox wrapping properties.
    layout().setFlexWrap(true); // Allow items to wrap to the next line.
    layout().setFlexReverseDirection(true); // Reverse the main axis direction.
    layout().setFlexWrapReverse(true); // Reverse the cross-axis direction for wrapped lines.

    int i = 0;
    for (Frame& frame : frames_) {
      addChild(&frame);

      // Set layout properties for each child frame.
      frame.layout().setHeight(100); // Fixed height.
      frame.layout().setWidth(100 + i * 10); // Variable initial width.

      // `setFlexGrow` determines how much a flex item will grow relative to the rest of the
      // flex items when there is extra space. Since all items have a grow factor of 1.0,
      // they will all share the extra space equally.
      frame.layout().setFlexGrow(1.0f);

      // Set a simple draw callback to visualize the frame.
      frame.onDraw() = [&frame](visage::Canvas& canvas) {
        canvas.setColor(0xff888888);
        canvas.roundedRectangle(0, 0, frame.width(), frame.height(), 16);
      };
      ++i;
    }
  }

  // Draw the main window's background.
  void draw(visage::Canvas& canvas) override {
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());
  }

private:
  // An array of frames to be laid out.
  Frame frames_[kNumFrames];
};

int runExample() {
  ExampleEditor editor;
  editor.show(visage::Dimension::logicalPixels(800), visage::Dimension::logicalPixels(600));
  editor.runEventLoop();

  return 0;
}
