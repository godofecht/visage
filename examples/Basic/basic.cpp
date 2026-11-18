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

int runExample() {
  // Create the main application window.
  // The ApplicationWindow class handles the window creation, event loop, and drawing context.
  visage::ApplicationWindow app;

  // Set the drawing callback.
  // This lambda function will be called for each frame to draw the window's content.
  app.onDraw() = [&app](visage::Canvas& canvas) {
    // Set the background color to a dark blue.
    // Colors are represented as 32-bit integers in ARGB format (0xAARRGGBB).
    canvas.setColor(0xff000066);
    // Fill the entire window with the current color.
    canvas.fill(0, 0, app.width(), app.height());

    // Calculate the properties of a circle to be drawn in the center of the window.
    float circle_radius = app.height() * 0.1f;
    float x = app.width() * 0.5f - circle_radius;
    float y = app.height() * 0.5f - circle_radius;

    // Set the circle color to cyan.
    canvas.setColor(0xff00ffff);
    // Draw the circle.
    canvas.circle(x, y, 2.0f * circle_radius);
  };

  // Set the title of the window.
  app.setTitle("Visage Basic Example");

  // Show the window with a specific size.
  app.show(800, 600);

  // Start the application's event loop.
  // This function will block until the window is closed.
  app.runEventLoop();

  return 0;
}
