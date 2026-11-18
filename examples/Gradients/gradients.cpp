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

/// Custom gradient definitions:
///
/// OkLab: https://bottosson.github.io/posts/oklab/
/// Viridis: https://sjmgarnier.github.io/viridis/articles/intro-to-viridis.html

#include "embedded/example_fonts.h"

#include <array>
#include <visage/app.h>
#include <visage/widgets.h>

using namespace visage::dimension;

// This example demonstrates how to create and use various gradients.
// It includes perceptually uniform colormaps like Viridis and OkLab.

// A sampling function that generates a rainbow gradient in the Oklab color space.
// Oklab is designed to be perceptually uniform, meaning changes in color values
// correspond to similar-sized changes in visual appearance.
visage::Color sampleOkLab(float t) {
  static constexpr float kPi = 3.14159265358979323846f;
  static constexpr float kL = 0.82f;
  static constexpr float kC = 0.15f;
  static constexpr float kOffset = 0.45f;

  // Convert from polar to cartesian coordinates in the a-b plane of Oklab.
  float a = kC * std::cos(2.0f * kPi * t + kOffset);
  float b = kC * std::sin(2.0f * kPi * t + kOffset);

  // Oklab to LMS color space conversion (cone fundamentals).
  float l_ = kL + 0.3963377774f * a + 0.2158037573f * b;
  float m_ = kL - 0.1055613458f * a - 0.0638541728f * b;
  float s_ = kL - 0.0894841775f * a - 1.2914855480f * b;

  float l = l_ * l_ * l_;
  float m = m_ * m_ * m_;
  float s = s_ * s_ * s_;

  // LMS to linear sRGB conversion.
  return { 1.0f, +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s,
           -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s,
           -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s };
}

// Helper function to configure a Gradient's wrapping behavior based on a string name.
void setPattern(visage::Gradient& gradient, const std::string& pattern) {
  if (pattern == "Single") {
    gradient.setRepeat(false); // The gradient is drawn once.
    gradient.setReflect(false);
  }
  else if (pattern == "Repeat") {
    gradient.setRepeat(true); // The gradient repeats itself.
    gradient.setReflect(false);
  }
  else { // Reflect
    gradient.setRepeat(false);
    gradient.setReflect(true); // The gradient reflects back and forth (mirrored).
  }
}

// --- Interactive Gradient Frames ---

// A base class for the interactive frames that display gradients.
// It handles drawing and dragging the two control points that define the gradient's geometry.
class PointsFrame : public visage::Frame {
public:
  static constexpr int kDragRadius = 20;
  static constexpr int kDotRadius = 5;
  enum ActivePoint {
    kNone,
    kFrom, // The start point of the gradient
    kTo    // The end point of the gradient
  };

  void resized() override {
    // Initialize the control points based on the frame's size.
    point1_ = visage::Point(width() * 0.33f, height() * 0.33f);
    point2_ = visage::Point(width() * 0.66f, height() * 0.66f);
  }

  void draw(visage::Canvas& canvas) override {
    // This base class draw method is overridden by subclasses, but provides a simple default.
    visage::Brush points = visage::Brush::linear(visage::Gradient(0xffffff00, 0xff00ffff), point1_, point2_);
    canvas.setColor(points);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    canvas.setColor(0xff000000);
    visage::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Linear Points", font, visage::Font::kCenter, 0, 0, width(), height());

    // Draw visual feedback for the draggable points.
    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);
    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }

  // --- Mouse Event Handling for Dragging Points ---

  void setActivePoint(ActivePoint active_point) {
    if (active_point == active_point_)
      return;
    active_point_ = active_point;
    redraw();
  }

  void mouseMove(const visage::MouseEvent& e) override {
    visage::Point point = e.position;
    visage::Point delta_from = point - point1_;
    visage::Point delta_to = point - point2_;

    // Determine which control point is being hovered over.
    if (delta_from.squareMagnitude() < kDragRadius * kDragRadius &&
        delta_from.squareMagnitude() < delta_to.squareMagnitude())
      setActivePoint(kFrom);
    else if (delta_to.squareMagnitude() < kDragRadius * kDragRadius)
      setActivePoint(kTo);
    else
      setActivePoint(kNone);
  }

  void mouseDown(const visage::MouseEvent& e) override {
    if (active_point_ == kNone)
      return;
    mouse_down_ = true;
    redraw();
  }

  void mouseUp(const visage::MouseEvent& e) override {
    if (active_point_ == kNone)
      return;
    mouse_down_ = false;
    redraw();
  }

  void mouseDrag(const visage::MouseEvent& e) override {
    if (active_point_ == kNone)
      return;
    // Update the position of the active control point.
    if (active_point_ == kFrom)
      point1_ = localBounds().clampPoint(e.position);
    else if (active_point_ == kTo)
      point2_ = localBounds().clampPoint(e.position);
    redraw();
  }

  // --- Public Setters ---

  void setGradient(const visage::Gradient& gradient) {
    gradient_ = gradient;
    redraw();
  }

  void setPoints(const visage::Point& point1, const visage::Point& point2) {
    point1_ = point1;
    point2_ = point2;
    redraw();
  }

protected:
  ActivePoint active_point_ = kNone;
  bool mouse_down_ = false;
  visage::Point point1_;
  visage::Point point2_;
  visage::Gradient gradient_;
};

// A specialization of PointsFrame that draws a linear gradient.
class LinearPointsFrame : public PointsFrame {
public:
  void draw(visage::Canvas& canvas) override {
    // Create a linear brush using the current gradient and the two control points.
    // The gradient will stretch between point1 and point2.
    visage::Brush points = visage::Brush::linear(gradient_, point1_, point2_);
    canvas.setColor(points);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    // Draw the descriptive text and the control points.
    canvas.setColor(0xff000000);
    visage::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Linear Gradient", font, visage::Font::kCenter, 0, 0, width(), height());

    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);
    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }
};

// A specialization of PointsFrame that draws a radial gradient.
class RadialPointsFrame : public PointsFrame {
public:
  void draw(visage::Canvas& canvas) override {
    // The radius of the radial gradient is the distance between the two control points.
    auto radius = (point2_ - point1_).length();
    // Create a radial brush. The gradient will emanate from point1 out to the calculated radius.
    visage::Brush radial = visage::Brush::radial(gradient_, point1_, radius);
    canvas.setColor(radial);
    canvas.roundedRectangle(0, 0, width(), height(), 18.0f);

    // Draw the descriptive text and the control points.
    canvas.setColor(0xff000000);
    visage::Font font(20, resources::fonts::Lato_Regular_ttf);
    canvas.text("Radial Gradient", font, visage::Font::kCenter, 0, 0, width(), height());

    canvas.setColor(mouse_down_ ? 0xaaffffff : 0x66ffffff);
    if (active_point_ == kFrom)
      canvas.circle(point1_.x - kDragRadius, point1_.y - kDragRadius, 2.0f * kDragRadius);
    else if (active_point_ == kTo)
      canvas.circle(point2_.x - kDragRadius, point2_.y - kDragRadius, 2.0f * kDragRadius);

    canvas.setColor(0xff000000);
    canvas.circle(point1_.x - kDotRadius, point1_.y - kDotRadius, 2.0f * kDotRadius);
    canvas.circle(point2_.x - kDotRadius, point2_.y - kDotRadius, 2.0f * kDotRadius);
  }
};

int runExample() {
  visage::ApplicationWindow app;

  // --- UI Layout Setup ---
  // The main window uses a vertical flexbox layout.
  app.layout().setFlex(true);
  app.layout().setFlexGap(8);
  app.layout().setPadding(8);

  // A frame to hold the two gradient displays, arranged horizontally.
  visage::Frame gradients;
  gradients.layout().setFlex(true);
  gradients.layout().setFlexRows(false);
  gradients.layout().setFlexGap(8);
  gradients.layout().setFlexGrow(1.0f);
  app.addChild(gradients);

  // Set a dark background for the main window.
  app.onDraw() = [&app](visage::Canvas& canvas) {
    canvas.setColor(0xff222222);
    canvas.fill(0, 0, app.width(), app.height());
  };

  // The initial gradient to be displayed.
  visage::Gradient gradient(0xffffff00, 0xff00aaff);

  // Create and add the linear gradient display.
  std::unique_ptr<PointsFrame> linear_points_frame = std::make_unique<LinearPointsFrame>();
  gradients.addChild(linear_points_frame.get());
  linear_points_frame->layout().setFlexGrow(1.0f);
  linear_points_frame->setGradient(gradient);

  // Create and add the radial gradient display.
  std::unique_ptr<PointsFrame> radial_points_frame = std::make_unique<RadialPointsFrame>();
  gradients.addChild(radial_points_frame.get());
  radial_points_frame->layout().setFlexGrow(1.0f);
  radial_points_frame->setGradient(gradient);

  // --- Control Buttons ---
  visage::Font font(20, resources::fonts::Lato_Regular_ttf);
  visage::Frame controls;
  controls.layout().setFlexGrow(0.15f);
  controls.layout().setFlex(true);
  controls.layout().setFlexRows(false);
  controls.layout().setFlexGap(8);

  // Button to cycle through gradient wrapping patterns (Single, Repeat, Reflect).
  std::string pattern = "Single";
  visage::UiButton pattern_button("Pattern: " + pattern);
  pattern_button.setFont(font);
  pattern_button.layout().setFlexGrow(1.0f);
  controls.addChild(pattern_button);

  pattern_button.onToggle() = [&](visage::Button* button, bool on) {
    if (pattern == "Single")
      pattern = "Repeat";
    else if (pattern == "Repeat")
      pattern = "Reflect";
    else
      pattern = "Single";

    setPattern(gradient, pattern);
    linear_points_frame->setGradient(gradient);
    radial_points_frame->setGradient(gradient);
    pattern_button.setText("Pattern: " + pattern);
  };

  // A map of available gradients to cycle through.
  std::map<std::string, visage::Gradient> gradients_map = {
    { "Two Color", visage::Gradient(0xffffff00, 0xff00aaff) },
    { "Rainbow", visage::Gradient(0xffff0000, 0xffffff00, 0xff00ff00, 0xff00ffff, 0xff0000ff,
                                  0xffff00ff, 0xffff0000) },
    // Gradients can also be created from a sampling function.
    { "Rainbow (OkLab)", visage::Gradient::fromSampleFunction(100, sampleOkLab) },
    { "Viridis", visage::Gradient::kViridis }
  };

  // Button to cycle through the different gradient color maps.
  visage::UiButton color_button("Gradient: Two Color");
  color_button.setFont(font);
  color_button.layout().setFlexGrow(1.0f);
  controls.addChild(color_button);

  app.addChild(controls);
  auto gradient_it = gradients_map.find("Two Color");
  color_button.onToggle() = [&](visage::Button* button, bool on) {
    gradient_it = std::next(gradient_it);
    if (gradient_it == gradients_map.end())
      gradient_it = gradients_map.begin();

    gradient = gradient_it->second;
    setPattern(gradient, pattern);
    linear_points_frame->setGradient(gradient);
    radial_points_frame->setGradient(gradient);
    color_button.setText("Gradient: " + gradient_it->first);
  };

  // --- Window Setup ---
  app.setTitle("Visage Gradient Example");
  // Use viewport-relative units to set the initial window size.
  app.show(80_vmin, 60_vmin);
  app.runEventLoop();
  return 0;
}
