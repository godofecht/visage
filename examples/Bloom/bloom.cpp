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
#include <visage_widgets/graph_line.h>

// This class defines a custom frame that draws an animated, glowing line and dots.
// The bloom effect is achieved by drawing with "HDR" colors (where brightness > 1.0)
// and then applying a BloomPostEffect to the main window.
class AnimatedLine : public visage::Frame {
public:
  static constexpr int kNumPoints = 1200;
  static constexpr float kDotRadius = 5.0f;

  AnimatedLine() : graph_line_(kNumPoints) {
    addChild(&graph_line_);
    setIgnoresMouseEvents(true, false);
  }

  void resized() override { graph_line_.setBounds(0, 0, width(), height()); }

  // Updates the points of the GraphLine to create a flowing, wavy animation.
  void setLinePositions(double render_time) {
    static constexpr float kPi = 3.14159265358979323846f;
    float position = 0.0f;
    double line_phase = render_time * 0.5;
    for (int i = 0; i < kNumPoints; ++i) {
      float t = 1.1f * i / (kNumPoints - 1.0f) - 0.05f;
      float delta = std::min(t, 1.0f - t);
      position += 0.02f * delta * delta + 0.003f;
      graph_line_.set(i, 0.5f + std::sin((line_phase + position) * 2.0f * kPi) * 0.25f);
    }
  }

  // Computes the Brush (color/gradient) for the line and dots.
  // This is the key to the bloom effect. The brush is created from a gradient
  // that has a brightness component greater than 1.0. The BloomPostEffect will
  // pick up these "overbright" pixels and create a glow around them.
  visage::Brush computeBrush(double render_time) const {
    // A rainbow gradient for the base color.
    visage::Gradient rainbow(0xffff6666, 0xffffff66, 0xff66ff66, 0xff66ffff, 0xff6666ff, 0xffff66ff,
                             0xffff6666);

    // A "boost" gradient that modulates the brightness.
    // The brightness value in the visage::Color constructor is the fifth argument.
    // Here, it's set to a value that pulses above 1.0, creating the HDR effect.
    float boost_time = render_time * 0.2f;
    float boost_phase = (boost_time - floor(boost_time)) * 1.5f - 0.25f;
    visage::Gradient boost = visage::Gradient::fromSampleFunction(256, [boost_phase](float t) {
      return visage::Color(1.0f, 1.0f, 1.0f, 1.0f,
                           1.0f + std::max(0.0f, 0.4f - 3.0f * std::abs(boost_phase - t)));
    });

    // The final brush is a linear gradient created by multiplying the color and brightness gradients.
    return visage::Brush::linear(rainbow * boost, visage::Point(0, 0), visage::Point(width(), 0));
  }

  // The main drawing method for this frame.
  void draw(visage::Canvas& canvas) override {
    static constexpr int kNumDots = 10;

    double render_time = canvas.time();
    setLinePositions(render_time);

    // Get the HDR brush and apply it to the canvas and the GraphLine's palette.
    visage::Brush brush = computeBrush(render_time);
    palette()->setColor(visage::GraphLine::LineColor, brush);
    canvas.setColor(brush);

    // Draw two rows of dots.
    int render_height = height();
    int render_width = width();

    float center_y = render_height * 0.125f;
    for (int i = 0; i < kNumDots; ++i) {
      float t = (i + 1) / (kNumDots + 1.0f);
      float center_x = t * render_width;

      canvas.circle(center_x - kDotRadius, center_y - kDotRadius, kDotRadius * 2.0f);
      canvas.circle(center_x - kDotRadius, render_height - center_y - kDotRadius, kDotRadius * 2.0f);
    }

    // Request a redraw for the next frame to continue the animation.
    redraw();
  }

private:
  visage::GraphLine graph_line_;
};

// The main application window for the Bloom example.
class ExampleEditor : public visage::ApplicationWindow {
public:
  ExampleEditor() {
    // --- Bloom Post-Effect Setup ---
    // A post-effect is a shader that runs on the entire window content after it has been drawn.
    // The BloomPostEffect creates a "glow" or "bloom" effect for bright areas of the image.
    bloom_.setBloomSize(30.0f); // The radius of the bloom in pixels.
    bloom_.setBloomIntensity(2.0f); // The brightness of the bloom.
    setPostEffect(&bloom_); // Apply the effect to this window.

    // Add the animated line frame as a child.
    addChild(&animated_line_);
    animated_line_.layout().setMargin(0);

    // Set the window's background color.
    onDraw() = [this](visage::Canvas& canvas) {
      canvas.setColor(0xff22282d);
      canvas.fill(0, 0, width(), height());
    };

    // Use a palette to set the line width for the GraphLine widget.
    setPalette(&palette_);
    palette_.setValue(visage::GraphLine::LineWidth, 2.5f);
  }

private:
  visage::Palette palette_;
  visage::BloomPostEffect bloom_;
  AnimatedLine animated_line_;
};

int runExample() {
  ExampleEditor editor;
  editor.setWindowDecoration(visage::Window::Decoration::Client);
  if (visage::isMobileDevice())
    editor.showMaximized();
  else
    editor.show(visage::Dimension::widthPercent(50.0f), visage::Dimension::widthPercent(14.0f));

  editor.runEventLoop();

  return 0;
}
