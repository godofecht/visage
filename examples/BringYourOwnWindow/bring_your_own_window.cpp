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

#include <visage/graphics.h>
#include <visage/windowing.h>

// This example demonstrates a lower-level approach to using Visage, where the
// application takes more direct control over the window and rendering loop.
// This is in contrast to the `ApplicationWindow` class, which abstracts away
// much of this setup. This approach is useful when integrating Visage into an
// existing application with its own windowing system.
int runExample() {
  // 1. Create a native window using Visage's windowing utilities.
  //    Alternatively, you could create a window using another library (e.g., SDL, Qt)
  //    and provide its native handle to the renderer.
  std::unique_ptr<visage::Window> window = visage::createWindow(800, 800);

  // 2. Initialize the global Renderer instance.
  //    `initWindow()` performs platform-specific setup for the graphics context.
  visage::Renderer::instance().initialize(window->initWindow(), window->globalDisplay());

  // 3. Create a Canvas for drawing.
  visage::Canvas canvas;
  //    Pair the canvas with the native window handle. This tells the canvas where to draw.
  canvas.pairToWindow(window->nativeHandle(), window->clientWidth(), window->clientHeight());

  // 4. Perform drawing operations on the canvas.
  canvas.setColor(0xff223333);
  canvas.fill(0, 0, window->clientWidth(), window->clientHeight());
  canvas.setColor(0xffaa99ff);
  canvas.ring(50, 50, window->clientWidth() - 100.0f, window->clientWidth() * 0.1f);

  // 5. Show the window and submit the drawing commands.
  window->show();
  canvas.submit(); // This sends the drawing commands to the GPU.

  // 6. Run the window's event loop.
  //    In a real application, you would likely have a more complex loop that
  //    handles events and triggers redraws as needed. For each redraw, you would
  //    repeat steps 4 and 5 (drawing and submitting).
  window->runEventLoop();

  return 0;
}
