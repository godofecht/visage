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

// This helper function draws three overlapping circles in red, green, and blue,
// resembling a Venn diagram. It's used to demonstrate how different blend modes
// affect overlapping colors.
static void drawRgbCircles(visage::Canvas& canvas, float width, float height) {
  // Define constants for circle layout.
  static constexpr float kCircleRadiusRatio = 0.2f;
  static constexpr float kVennRadiusRatio = 0.13f;
  static constexpr float k60DegreeTriangleRatio = 0.866025403784f; // sin(60 degrees)

  // Base dimensions on the smaller of width or height to maintain aspect ratio.
  float min_dimension = std::min(width, height);

  // Calculate pixel dimensions from ratios.
  float circle_radius = kCircleRadiusRatio * min_dimension;
  float venn_radius = kVennRadiusRatio * min_dimension;
  float venn_offset = k60DegreeTriangleRatio * venn_radius;
  float start_x = width / 2.0f - circle_radius;
  float start_y = height / 2.0f - circle_radius;

  // Draw the three circles with their respective colors.
  canvas.setColor(0xffff0000); // Red
  canvas.circle(start_x, start_y + venn_radius, 2 * circle_radius);
  canvas.setColor(0xff00ff00); // Green
  canvas.circle(start_x - venn_offset, start_y - venn_radius * 0.5f, 2 * circle_radius);
  canvas.setColor(0xff0000ff); // Blue
  canvas.circle(start_x + venn_offset, start_y - venn_radius * 0.5f, 2 * circle_radius);
}

// The main application window class for this example.
// It is divided into four quadrants, each demonstrating a different blending concept.
class ExampleEditor : public visage::ApplicationWindow {
public:
  ExampleEditor() {
    // Add four child frames to the window, one for each quadrant.
    addChild(&additive_frame_);
    addChild(&subtractive_frame_);

    // The masked frame demonstrates using blend modes to create a mask.
    masked_frame_.setMasked(true);
    addChild(&masked_frame_);

    // This frame demonstrates grouped transparency. The entire frame and its children
    // will be rendered with an alpha of 0.5.
    transparent_frame_.setAlphaTransparency(0.5f);
    addChild(&transparent_frame_);

    // --- Top-Left Quadrant: Additive Blending ---
    additive_frame_.onDraw() = [this](visage::Canvas& canvas) {
      // Set the blend mode to Add. In this mode, color values are added together.
      // For RGB colors, this simulates mixing light (e.g., red + green = yellow).
      canvas.setBlendMode(visage::BlendMode::Add);
      canvas.setColor(0xffffffff);
      canvas.text("Additive", font_, visage::Font::kCenter, 0, 0, additive_frame_.width(),
                  0.2f * additive_frame_.height());
      drawRgbCircles(canvas, additive_frame_.width(), additive_frame_.height());
    };

    // --- Top-Right Quadrant: Subtractive Blending ---
    subtractive_frame_.onDraw() = [this](visage::Canvas& canvas) {
      // Start with a light gray background.
      canvas.setColor(0xffeeeeee);
      canvas.fill(0, 0, subtractive_frame_.width(), subtractive_frame_.height());

      // Set the blend mode to Sub. Color values are subtracted.
      // This simulates mixing pigments (e.g., white - red = cyan).
      canvas.setBlendMode(visage::BlendMode::Sub);
      canvas.setColor(0xffffffff);
      canvas.text("Subtractive", font_, visage::Font::kCenter, 0, 0, subtractive_frame_.width(),
                  0.2f * subtractive_frame_.height());
      drawRgbCircles(canvas, subtractive_frame_.width(), subtractive_frame_.height());
    };

    // --- Bottom-Left Quadrant: Masking ---
    masked_frame_.onDraw() = [this](visage::Canvas& canvas) {
      // First, draw a background to be masked. Here it's magenta and white stripes.
      static constexpr int kColumns = 12;
      int w = masked_frame_.width();
      int h = masked_frame_.height();
      int last_x = 0;
      int color1 = 0xffff00ff;
      int color2 = 0xffffffff;
      for (int i = 0; i < kColumns; ++i) {
        int x = w * (i + 1) / kColumns;
        canvas.setColor(color1);
        std::swap(color1, color2);
        canvas.fill(last_x, 0, x - last_x, h);
        last_x = x;
      }

      // To create a mask, we first clear the mask channel.
      canvas.setBlendMode(visage::BlendMode::MaskRemove);
      canvas.setColor(0xffffffff);
      canvas.fill(0, 0, w, h);

      // Then, we draw the shape of our mask.
      canvas.setBlendMode(visage::BlendMode::MaskAdd);
      canvas.setColor(0xffffffff);
      drawRgbCircles(canvas, w, h);

      // Finally, we set the blend mode back to Alpha to draw content on top,
      // which will now be clipped by the mask we created.
      canvas.setBlendMode(visage::BlendMode::Alpha);
      canvas.setColor(0xffffffff);
      canvas.text("Masked", font_, visage::Font::kCenter, 0, 0, w, 0.2f * masked_frame_.height());
    };

    // --- Bottom-Right Quadrant: Grouped Transparency ---
    transparent_frame_.onDraw() = [this](visage::Canvas& canvas) {
      // All drawing within this frame will be affected by the `setAlphaTransparency(0.5f)`
      // call made in the constructor. The entire frame is rendered to a separate texture
      // and then blended with the main window with 50% opacity.
      canvas.setColor(0xffffffff);
      canvas.text("Grouped Transparency", font_, visage::Font::kCenter, 0, 0,
                  transparent_frame_.width(), 0.2f * transparent_frame_.height());
      drawRgbCircles(canvas, transparent_frame_.width(), transparent_frame_.height());
    };
  }

  // The main draw call for the window.
  void draw(visage::Canvas& canvas) override {
    // Set a dark background for the entire window.
    canvas.setColor(0xff222026);
    canvas.fill(0, 0, width(), height());

    // Draw a checkered background behind the transparent frame to make its effect visible.
    static constexpr int kColumns = 12;
    int w = transparent_frame_.width();
    int h = transparent_frame_.height();
    int last_x = transparent_frame_.x();
    int y = transparent_frame_.y();
    int color1 = 0xff666666;
    int color2 = 0xff888888;
    for (int i = 0; i < kColumns; ++i) {
      int x = transparent_frame_.x() + w * (i + 1) / kColumns;
      canvas.setColor(color1);
      std::swap(color1, color2);
      canvas.fill(last_x, y, x - last_x, h);
      last_x = x;
    }
  }

  // This is called when the window is resized.
  void resized() override {
    // Recalculate the bounds of the four quadrants.
    int center_x = width() / 2;
    int center_y = height() / 2;

    additive_frame_.setBounds(0, 0, center_x, center_y);
    subtractive_frame_.setBounds(center_x, 0, width() - center_x, center_y);
    masked_frame_.setBounds(0, center_y, center_x, height() - center_y);
    transparent_frame_.setBounds(center_x, center_y, width() - center_x, height() - center_y);

    // Re-create the font with the appropriate DPI scaling.
    font_ = visage::Font(16, resources::fonts::Lato_Regular_ttf);
  }

private:
  Frame additive_frame_;
  Frame subtractive_frame_;
  Frame masked_frame_;
  Frame transparent_frame_;
  visage::Font font_;
};

int runExample() {
  ExampleEditor editor;
  if (visage::isMobileDevice())
    editor.showMaximized();
  else
    editor.show(visage::Dimension::logicalPixels(800), visage::Dimension::logicalPixels(600));
  editor.runEventLoop();

  return 0;
}
