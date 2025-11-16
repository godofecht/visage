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
#include <visage/graphics.h>
#include <visage/ui.h>

// This function creates a Path object representing a five-pointed star.
visage::Path starPath(float center_x, float center_y, float radius) {
  static constexpr float kPi = 3.14159265f;
  visage::Path path;
  int num_points = 10; // 5 inner points and 5 outer points

  // Loop to calculate the vertices of the star.
  for (int i = 0; i < num_points; ++i) {
    float angle = (float)i / (float)num_points * 2.0f * kPi;
    // Alternate between the outer radius and a smaller inner radius.
    float r = (i % 2) ? radius : radius * 0.4f;
    auto point = visage::Point { center_x, center_y } +
                 r * visage::Point(std::sin(angle), std::cos(angle));

    // The first point uses moveTo to start the path.
    if (i == 0)
      path.moveTo(point);
    // Subsequent points use lineTo to draw a line from the previous point.
    else
      path.lineTo(point);
  }
  // close() connects the last point back to the first point.
  path.close();
  return path;
}

int runExample() {
  visage::ApplicationWindow app;

  app.onDraw() = [&](visage::Canvas& canvas) {
    // Set a dark background.
    canvas.setColor(0xff222222);
    canvas.fill(0, 0, app.width(), app.height());

    // Set the drawing color to magenta.
    canvas.setColor(0xffff44ff);

    // Divide the window into three vertical sections to show different path renderings.
    float w = app.width() / 3.0f;
    float h = app.height();

    // Create the base star path.
    visage::Path star = starPath(w * 0.5f, h * 0.5f, std::min(w, h) * 0.4f);

    // --- Path Variations ---

    // 1. A stroked path: This creates a new path that represents the outline of the original star.
    auto stroked = star.stroke(2); // The argument is the stroke width.

    // 2. A dashed path: This also creates a stroked path, but with a dash pattern.
    float segment = star.length() / 20.0f;
    // The `stroke` method has optional arguments for join style, end cap style, a dash pattern,
    // and a dash offset. Here, we create a simple dash pattern and animate the offset over time.
    auto dashed = star.stroke(2, visage::Path::Join::Miter, visage::Path::EndCap::Butt, { segment },
                              canvas.time() * segment);

    // --- Drawing ---

    // Draw the original filled star in the left section.
    canvas.fill(star);

    // Draw the stroked star in the middle section. Note the x-offset in the fill command.
    canvas.fill(stroked, w, 0);

    // Draw the animated dashed star in the right section.
    canvas.fill(dashed, 2.0f * w, 0);

    // Request a redraw to continue the animation of the dashed line.
    app.redraw();
  };

  app.setTitle("Visage Paths Example");
  app.show(900, 200);
  app.runEventLoop();
  return 0;
}
