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

#include "events.h"
#include "layout.h"
#include "undo_history.h"
#include "visage_graphics/canvas.h"
#include "visage_graphics/palette.h"
#include "visage_graphics/theme.h"
#include "visage_utils/space.h"

#include <string>
#include <vector>

namespace visage {
  class Frame;
  /**
   * @struct FrameEventHandler
   * @brief A collection of callbacks for handling events within a Frame.
   *
   * This structure holds function pointers that can be set to handle various
   * events, such as requesting a redraw or changing the mouse cursor.
   */
  struct FrameEventHandler {
    std::function<void(Frame*)> request_redraw = nullptr;
    std::function<void(Frame*)> request_keyboard_focus = nullptr;
    std::function<void(Frame*)> remove_from_hierarchy = nullptr;
    std::function<void(bool)> set_mouse_relative_mode = nullptr;
    std::function<void(MouseCursor)> set_cursor_style = nullptr;
    std::function<void(bool)> set_cursor_visible = nullptr;
    std::function<std::string()> read_clipboard_text = nullptr;
    std::function<void(std::string)> set_clipboard_text = nullptr;
  };
  /**
   * @class Frame
   * @brief The base class for all UI components in Visage.
   *
   * Frame represents a rectangular area on the screen and is the fundamental
   * building block for creating user interfaces. It can contain other Frames,
   * forming a hierarchy. It handles drawing, layout, and events.
   */
  class Frame {
  public:
    /**
     * @brief Default constructor.
     */
    Frame() = default;
    /**
     * @brief Constructs a Frame with a given name.
     * @param name The name of the frame.
     */
    explicit Frame(std::string name) : name_(std::move(name)) { }
    /**
     * @brief Virtual destructor.
     */
    virtual ~Frame() {
      notifyRemoveFromHierarchy();
      if (parent_)
        parent_->eraseChild(this);

      removeAllChildren();
    }
    /**
     * @brief Returns the callback list for the draw event.
     *
     * The draw event is triggered when the frame needs to be redrawn.
     * The callback function receives a reference to the Canvas to draw on.
     *
     * @return A reference to the on_draw_ callback list.
     */
    /**
     * @brief Returns the callback list for the draw event.
     *
     * The draw event is triggered when the frame needs to be redrawn.
     * The callback function receives a reference to the Canvas to draw on.
     *
     * @return A reference to the on_draw_ callback list.
     */
    auto& onDraw() { return on_draw_; }
    /**
     * @brief Returns the callback list for the resize event.
     * @return A reference to the on_resize_ callback list.
     */
    auto& onResize() { return on_resize_; }
    /**
     * @brief Returns the callback list for when a child's bounds change.
     * @return A reference to the on_child_bounds_changed_ callback list.
     */
    auto& onChildBoundsChanged() { return on_child_bounds_changed_; }
    /**
     * @brief Returns the callback list for when a child is added.
     * @return A reference to the on_child_added_ callback list.
     */
    auto& onChildAdded() { return on_child_added_; }
    /**
     * @brief Returns the callback list for when a child is removed.
     * @return A reference to the on_child_removed_ callback list.
     */
    auto& onChildRemoved() { return on_child_removed_; }
    /**
     * @brief Returns the callback list for when the DPI scale changes.
     * @return A reference to the on_dpi_change_ callback list.
     */
    auto& onDpiChange() { return on_dpi_change_; }
    /**
     * @brief Returns the callback list for when the visibility changes.
     * @return A reference to the on_visibility_change_ callback list.
     */
    auto& onVisibilityChange() { return on_visibility_change_; }
    /**
     * @brief Returns the callback list for when the hierarchy changes.
     * @return A reference to the on_hierarchy_change_ callback list.
     */
    auto& onHierarchyChange() { return on_hierarchy_change_; }
    /**
     * @brief Returns the callback list for when the focus changes.
     * @return A reference to the on_focus_change_ callback list.
     */
    auto& onFocusChange() { return on_focus_change_; }
    /**
     * @brief Returns the callback list for the mouse enter event.
     * @return A reference to the on_mouse_enter_ callback list.
     */
    auto& onMouseEnter() { return on_mouse_enter_; }
    /**
     * @brief Returns the callback list for the mouse exit event.
     * @return A reference to the on_mouse_exit_ callback list.
     */
    auto& onMouseExit() { return on_mouse_exit_; }
    /**
     * @brief Returns the callback list for the mouse down event.
     * @return A reference to the on_mouse_down_ callback list.
     */
    auto& onMouseDown() { return on_mouse_down_; }
    /**
     * @brief Returns the callback list for the mouse up event.
     * @return A reference to the on_mouse_up_ callback list.
     */
    auto& onMouseUp() { return on_mouse_up_; }
    /**
     * @brief Returns the callback list for the mouse move event.
     * @return A reference to the on_mouse_move_ callback list.
     */
    auto& onMouseMove() { return on_mouse_move_; }
    /**
     * @brief Returns the callback list for the mouse drag event.
     * @return A reference to the on_mouse_drag_ callback list.
     */
    auto& onMouseDrag() { return on_mouse_drag_; }
    /**
     * @brief Returns the callback list for the mouse wheel event.
     * @return A reference to the on_mouse_wheel_ callback list.
     */
    auto& onMouseWheel() { return on_mouse_wheel_; }
    /**
     * @brief Returns the callback list for the key press event.
     * @return A reference to the on_key_press_ callback list.
     */
    auto& onKeyPress() { return on_key_press_; }
    /**
     * @brief Returns the callback list for the key release event.
     * @return A reference to the on_key_release_ callback list.
     */
    auto& onKeyRelease() { return on_key_release_; }
    /**
     * @brief Returns the callback list for the text input event.
     * @return A reference to the on_text_input_ callback list.
     */
    auto& onTextInput() { return on_text_input_; }
    /**
     * @brief Initializes the frame and its children.
     */
    virtual void init() { initChildren(); }
    /**
     * @brief Draws the frame on the given canvas.
     * @param canvas The canvas to draw on.
     */
    virtual void draw(Canvas& canvas) { }
    /**
     * @brief Destroys the frame and its children.
     */
    virtual void destroy() { destroyChildren(); }
    /**
     * @brief Called when the frame is resized.
     */
    virtual void resized() { }
    /**
     * @brief Called when the DPI scale of the frame changes.
     */
    virtual void dpiChanged() { }
    /**
     * @brief Called when the visibility of the frame changes.
     */
    virtual void visibilityChanged() { }
    /**
     * @brief Called when the frame's position in the hierarchy changes.
     */
    virtual void hierarchyChanged() { }
    /**
     * @brief Called when the focus state of the frame changes.
     * @param is_focused True if the frame has gained focus, false otherwise.
     * @param was_clicked True if the focus change was due to a mouse click.
     */
    virtual void focusChanged(bool is_focused, bool was_clicked) { }
    /**
     * @brief Called when the mouse enters the frame.
     * @param e The mouse event.
     */
    virtual void mouseEnter(const MouseEvent& e) { }
    /**
     * @brief Called when the mouse exits the frame.
     * @param e The mouse event.
     */
    virtual void mouseExit(const MouseEvent& e) { }
    /**
     * @brief Called when a mouse button is pressed down on the frame.
     * @param e The mouse event.
     */
    virtual void mouseDown(const MouseEvent& e) { }
    /**
     * @brief Called when a mouse button is released on the frame.
     * @param e The mouse event.
     */
    virtual void mouseUp(const MouseEvent& e) { }
    /**
     * @brief Called when the mouse is moved over the frame.
     * @param e The mouse event.
     */
    virtual void mouseMove(const MouseEvent& e) { }
    /**
     * @brief Called when the mouse is dragged over the frame.
     * @param e The mouse event.
     */
    virtual void mouseDrag(const MouseEvent& e) { }
    /**
     * @brief Called when the mouse wheel is scrolled over the frame.
     * @param e The mouse event.
     * @return True if the event was handled, false otherwise.
     */
    virtual bool mouseWheel(const MouseEvent& e) { return false; }
    /**
     * @brief Called when a key is pressed while the frame has focus.
     * @param e The key event.
     * @return True if the event was handled, false otherwise.
     */
    virtual bool keyPress(const KeyEvent& e) { return false; }
    /**
     * @brief Called when a key is released while the frame has focus.
     * @param e The key event.
     * @return True if the event was handled, false otherwise.
     */
    virtual bool keyRelease(const KeyEvent& e) { return false; }
    /**
     * @brief Checks if the frame receives text input.
     * @return True if the frame receives text input, false otherwise.
     */
    virtual bool receivesTextInput() { return false; }
    /**
     * @brief Called when text is input into the frame.
     * @param text The text that was input.
     */
    virtual void textInput(const std::string& text) { }
    /**
     * @brief Performs a hit test at the given position.
     * @param position The position to test.
     * @return The hit test result.
     */
    virtual HitTestResult hitTest(const Point& position) const { return HitTestResult::Client; }
    /**
     * @brief Checks if the frame receives dropped files.
     * @return True if the frame receives dropped files, false otherwise.
     */
    virtual bool receivesDragDropFiles() { return false; }
    /**
     * @brief Gets the regex for file extensions that can be dropped.
     * @return A string containing the regex.
     */
    virtual std::string dragDropFileExtensionRegex() { return ".*"; }
    /**
     * @brief Checks if the frame can receive multiple dropped files.
     * @return True if the frame can receive multiple files, false otherwise.
     */
    virtual bool receivesMultipleDragDropFiles() { return false; }
    /**
     * @brief Called when dragged files enter the frame.
     * @param paths A vector of strings containing the paths of the files.
     */
    virtual void dragFilesEnter(const std::vector<std::string>& paths) { }
    /**
     * @brief Called when dragged files exit the frame.
     */
    virtual void dragFilesExit() { }
    /**
     * @brief Called when files are dropped on the frame.
     * @param paths A vector of strings containing the paths of the files.
     */
    virtual void dropFiles(const std::vector<std::string>& paths) { }
    /**
     * @brief Checks if the frame is a drag and drop source.
     * @return True if the frame is a drag and drop source, false otherwise.
     */
    virtual bool isDragDropSource() { return false; }
    /**
     * @brief Called when a drag and drop operation starts from this frame.
     * @return A string representing the data being dragged.
     */
    virtual std::string startDragDropSource() { return ""; }
    /**
     * @brief Called to clean up after a drag and drop operation.
     */
    virtual void cleanupDragDropSource() { }
    /**
     * @brief Sets the palette for the frame and its children.
     * @param palette A pointer to the Palette to use.
     */
    void setPalette(Palette* palette) {
      palette_ = palette;
      for (Frame* child : children_)
        child->setPalette(palette);
    }

    /**
     * @brief Gets the palette used by the frame.
     * @return A pointer to the Palette.
     */
    Palette* palette() const { return palette_; }
    /**
     * @brief Sets a palette override for the frame.
     * @param override_id The ID of the theme override.
     * @param recursive If true, applies the override to all children recursively.
     */
    void setPaletteOverride(theme::OverrideId override_id, bool recursive = true) {
      palette_override_ = override_id;
      if (recursive) {
        for (Frame* child : children_)
          child->setPaletteOverride(override_id, true);
      }
    }
    /**
     * @brief Gets the palette override ID for the frame.
     * @return The theme override ID.
     */
    theme::OverrideId paletteOverride() const { return palette_override_; }
    /**
     * @brief Checks if the frame has been initialized.
     * @return True if the frame is initialized, false otherwise.
     */
    bool initialized() const { return initialized_; }
    /**
     * @brief Requests a redraw of the frame.
     */
    void redraw() {
      if (isVisible() && isDrawing() && !redrawing_)
        redrawing_ = requestRedraw();
    }
    /**
     * @brief Redraws the frame and all of its children.
     */
    void redrawAll() {
      redraw();
      for (Frame* child : children_)
        child->redrawAll();
    }
    /**
     * @brief Gets the drawing region of the frame.
     * @return A pointer to the Region.
     */
    Region* region() { return &region_; }
    /**
     * @brief Sets a post-processing effect for the frame.
     * @param post_effect A pointer to the PostEffect.
     */
    void setPostEffect(PostEffect* post_effect);
    /**
     * @brief Gets the current post-processing effect.
     * @return A pointer to the PostEffect.
     */
    PostEffect* postEffect() const { return post_effect_; }
    /**
     * @brief Removes the post-processing effect from the frame.
     */
    void removePostEffect();
    /**
     * @brief Sets a backdrop effect for the frame.
     * @param backdrop_effect A pointer to the PostEffect to use as a backdrop.
     */
    void setBackdropEffect(PostEffect* backdrop_effect);
    /**
     * @brief Gets the current backdrop effect.
     * @return A pointer to the PostEffect.
     */
    PostEffect* backdropEffect() const { return backdrop_effect_; }
    /**
     * @brief Removes the backdrop effect from the frame.
     */
    void removeBackdropEffect();
    /**
     * @brief Sets the blur radius for the frame.
     *
     * This will add a blur post-processing effect to the frame if it doesn't
     * already have one.
     *
     * @param blur_radius The radius of the blur.
     */
    void setBlurRadius(float blur_radius) {
      if (blur_radius <= 0.0f) {
        if (blur_effect_ && post_effect_ == blur_effect_.get())
          removePostEffect();

        blur_effect_ = nullptr;
        return;
      }

      if (blur_effect_ == nullptr) {
        blur_effect_ = std::make_unique<BlurPostEffect>();
        setPostEffect(blur_effect_.get());
      }

      blur_effect_->setBlurRadius(blur_radius);
      redraw();
    }
    /**
     * @brief Sets the alpha transparency of the frame.
     * @param alpha The alpha value, from 0.0 (fully transparent) to 1.0 (fully opaque).
     */
    void setAlphaTransparency(float alpha) {
      if (alpha_transparency_ == alpha)
        return;

      alpha_transparency_ = alpha;
      redraw();
    }
    /**
     * @brief Removes the alpha transparency from the frame.
     */
    void removeAlphaTransparency() { setAlphaTransparency(1.0f); }
    /**
     * @brief Sets whether the frame's contents should be cached.
     * @param cached True to cache the frame, false otherwise.
     */
    void setCached(bool cached) {
      cached_ = cached;
      redraw();
    }
    /**
     * @brief Sets whether the frame should be masked.
     * @param masked True to mask the frame, false otherwise.
     */
    void setMasked(bool masked) {
      masked_ = masked;
      redraw();
    }
    /**
     * @brief Gets the name of the frame.
     * @return A const reference to the frame's name.
     */
    const std::string& name() const { return name_; }
    /**
     * @brief Sets the name of the frame.
     * @param name The new name for the frame.
     */
    void setName(std::string name) { name_ = std::move(name); }
    /**
     * @brief Sets the visibility of the frame.
     * @param visible True to make the frame visible, false to hide it.
     */
    void setVisible(bool visible);
    /**
     * @brief Checks if the frame is visible.
     * @return True if the frame is visible, false otherwise.
     */
    bool isVisible() const { return visible_; }
    /**
     * @brief Sets whether the frame is drawn.
     * @param drawing True to draw the frame, false otherwise.
     */
    void setDrawing(bool drawing);
    /**
     * @brief Checks if the frame is drawn.
     * @return True if the frame is drawn, false otherwise.
     */
    bool isDrawing() const { return drawing_; }
    /**
     * @brief Sets whether the frame should be on top of its siblings.
     * @param on_top True to place the frame on top, false otherwise.
     */
    void setOnTop(bool on_top);
    /**
     * @brief Checks if the frame is on top of its siblings.
     * @return True if the frame is on top, false otherwise.
     */
    bool isOnTop() const { return on_top_; }
    /**
     * @brief Adds a child frame to this frame.
     * @param child A pointer to the child frame to add.
     * @param make_visible If true, the child frame will be made visible.
     */
    void addChild(Frame* child, bool make_visible = true);
    /**
     * @brief Adds a child frame to this frame.
     * @param child A reference to the child frame to add.
     * @param make_visible If true, the child frame will be made visible.
     */
    void addChild(Frame& child, bool make_visible = true) { addChild(&child, make_visible); }
    /**
     * @brief Adds a child frame to this frame, taking ownership of it.
     * @param child A unique_ptr to the child frame to add.
     * @param make_visible If true, the child frame will be made visible.
     */
    void addChild(std::unique_ptr<Frame> child, bool make_visible = true);
    /**
     * @brief Removes a child frame from this frame.
     * @param child A pointer to the child frame to remove.
     */
    void removeChild(Frame* child);
    /**
     * @brief Removes all child frames from this frame.
     */
    void removeAllChildren();
    /**
     * @brief Gets the index of a child frame.
     * @param child A pointer to the child frame.
     * @return The index of the child frame, or -1 if not found.
     */
    int indexOfChild(const Frame* child) const;
    /**
     * @brief Sets the parent of this frame.
     * @param parent A pointer to the parent frame.
     */
    void setParent(Frame* parent) {
      VISAGE_ASSERT(parent != this);

      parent_ = parent;
      if (parent && parent->palette())
        setPalette(parent->palette());
    }
    /**
     * @brief Gets the parent of this frame.
     * @return A pointer to the parent frame.
     */
    Frame* parent() const { return parent_; }
    /**
     * @brief Gets the children of this frame.
     * @return A const reference to the vector of child frames.
     */
    const std::vector<Frame*>& children() const { return children_; }
    /**
     * @brief Sets the event handler for this frame and its children.
     * @param handler A pointer to the FrameEventHandler.
     */
    void setEventHandler(FrameEventHandler* handler) {
      event_handler_ = handler;
      for (Frame* child : children_)
        child->setEventHandler(handler);
    }
    /**
     * @brief Gets the event handler for this frame.
     * @return A pointer to the FrameEventHandler.
     */
    FrameEventHandler* eventHandler() const { return event_handler_; }
    /**
     * @brief Finds the first parent of this frame that is of a given type.
     * @tparam T The type of the parent to find.
     * @return A pointer to the parent frame, or nullptr if not found.
     */
    template<typename T>
    T* findParent() const {
      Frame* frame = parent_;
      while (frame) {
        T* casted = dynamic_cast<T*>(frame);
        if (casted)
          return casted;

        frame = frame->parent_;
      }

      return nullptr;
    }
    /**
     * @brief Checks if the frame contains a given point.
     * @param point The point to check.
     * @return True if the frame contains the point, false otherwise.
     */
    bool containsPoint(Point point) const { return bounds_.contains(point); }
    /**
     * @brief Gets the topmost frame at a given point.
     * @param point The point to check.
     * @return A pointer to the frame at the given point, or nullptr if not found.
     */
    Frame* frameAtPoint(Point point);
    /**
     * @brief Gets the top-level parent of this frame.
     * @return A pointer to the top-level parent frame.
     */
    Frame* topParentFrame();
    /**
     * @brief Sets the bounds of this frame.
     * @param bounds The new bounds for the frame.
     */
    void setBounds(Bounds bounds);
    /**
     * @brief Sets the bounds of this frame.
     * @param x The x-coordinate of the top-left corner.
     * @param y The y-coordinate of the top-left corner.
     * @param width The width of the frame.
     * @param height The height of the frame.
     */
    void setBounds(float x, float y, float width, float height) {
      setBounds({ x, y, width, height });
    }
    /**
     * @brief Sets the native bounds of this frame.
     * @param native_bounds The new native bounds for the frame.
     */
    void setNativeBounds(IBounds native_bounds);
    /**
     * @brief Sets the native bounds of this frame.
     * @param x The x-coordinate of the top-left corner in native pixels.
     * @param y The y-coordinate of the top-left corner in native pixels.
     * @param width The width of the frame in native pixels.
     * @param height The height of the frame in native pixels.
     */
    void setNativeBounds(int x, int y, int width, int height) {
      setNativeBounds({ x, y, width, height });
    }
    /**
     * @brief Computes the layout bounding box for this frame.
     * @param bounds The bounds to use for the computation.
     * @return The computed layout bounding box.
     */
    IBounds computeLayoutBoundingBox(IBounds bounds) const;
    /**
     * @brief Computes the layout for this frame and its children.
     */
    void computeLayout();
    /**
     * @brief Computes the layout for a specific child of this frame.
     * @param child The child frame to compute the layout for.
     */
    void computeLayout(Frame* child);
    /**
     * @brief Gets the bounds of this frame.
     * @return A const reference to the bounds of the frame.
     */
    const Bounds& bounds() const { return bounds_; }
    /**
     * @brief Sets the top-left position of this frame.
     * @param x The new x-coordinate.
     * @param y The new y-coordinate.
     */
    void setTopLeft(float x, float y) { setBounds(x, y, width(), height()); }
    /**
     * @brief Gets the top-left position of this frame.
     * @return The top-left position.
     */
    Point topLeft() const { return { bounds_.x(), bounds_.y() }; }
    /**
     * @brief Checks if this frame has a layout.
     * @return True if the frame has a layout, false otherwise.
     */
    bool hasLayout() const { return layout_ != nullptr; }
    /**
     * @brief Gets the layout of this frame.
     * @return A reference to the layout.
     */
    Layout& layout() {
      if (layout_ == nullptr)
        layout_ = std::make_unique<Layout>();
      return *layout_;
    }
    /**
     * @brief Clears the layout of this frame.
     */
    void clearLayout() { layout_ = nullptr; }
    /**
     * @brief Sets whether this frame uses a flex layout.
     * @param flex True to use a flex layout, false otherwise.
     */
    void setFlexLayout(bool flex) { layout().setFlex(flex); }
    /**
     * @brief Gets the x-coordinate of this frame.
     * @return The x-coordinate.
     */
    float x() const { return bounds_.x(); }
    /**
     * @brief Gets the y-coordinate of this frame.
     * @return The y-coordinate.
     */
    float y() const { return bounds_.y(); }
    /**
     * @brief Gets the width of this frame.
     * @return The width.
     */
    float width() const { return bounds_.width(); }
    /**
     * @brief Gets the height of this frame.
     * @return The height.
     */
    float height() const { return bounds_.height(); }
    /**
     * @brief Gets the right edge coordinate of this frame.
     * @return The right edge coordinate.
     */
    float right() const { return bounds_.right(); }
    /**
     * @brief Gets the bottom edge coordinate of this frame.
     * @return The bottom edge coordinate.
     */
    float bottom() const { return bounds_.bottom(); }
    /**
     * @brief Gets the native x-coordinate of this frame.
     * @return The native x-coordinate.
     */
    int nativeX() const { return native_bounds_.x(); }
    /**
     * @brief Gets the native y-coordinate of this frame.
     * @return The native y-coordinate.
     */
    int nativeY() const { return native_bounds_.y(); }
    /**
     * @brief Gets the native width of this frame.
     * @return The native width.
     */
    int nativeWidth() const { return native_bounds_.width(); }
    /**
     * @brief Gets the native height of this frame.
     * @return The native height.
     */
    int nativeHeight() const { return native_bounds_.height(); }
    /**
     * @brief Gets the native right edge coordinate of this frame.
     * @return The native right edge coordinate.
     */
    int nativeRight() const { return native_bounds_.right(); }
    /**
     * @brief Gets the native bottom edge coordinate of this frame.
     * @return The native bottom edge coordinate.
     */
    int nativeBottom() const { return native_bounds_.bottom(); }
    /**
     * @brief Gets the aspect ratio of this frame.
     * @return The aspect ratio.
     */
    virtual float aspectRatio() const {
      return bounds_.width() * 1.0f / std::max(0.01f, bounds_.height());
    }
    /**
     * @brief Gets the local bounds of this frame.
     * @return The local bounds.
     */
    Bounds localBounds() const { return { 0.0f, 0.0f, width(), height() }; }
    /**
     * @brief Gets the native local bounds of this frame.
     * @return The native local bounds.
     */
    IBounds nativeLocalBounds() const { return { 0, 0, nativeWidth(), nativeHeight() }; }
    /**
     * @brief Gets the position of this frame in window coordinates.
     * @return The position in window coordinates.
     */
    Point positionInWindow() const;
    /**
     * @brief Gets the bounds of this frame relative to another frame.
     * @param other The other frame.
     * @return The relative bounds.
     */
    Bounds relativeBounds(const Frame* other) const;
    /**
     * @brief Checks if this frame accepts keystrokes.
     * @return True if the frame accepts keystrokes, false otherwise.
     */
    bool acceptsKeystrokes() const { return accepts_keystrokes_; }
    /**
     * @brief Sets whether this frame accepts keystrokes.
     * @param accepts_keystrokes True to accept keystrokes, false otherwise.
     */
    void setAcceptsKeystrokes(bool accepts_keystrokes) { accepts_keystrokes_ = accepts_keystrokes; }
    /**
     * @brief Checks if this frame receives mouse events from its children.
     * @return True if the frame receives child mouse events, false otherwise.
     */
    bool receiveChildMouseEvents() const { return receive_child_mouse_events_; }
    /**
     * @brief Sets whether this frame receives mouse events from its children.
     * @param receive True to receive child mouse events, false otherwise.
     */
    void setReceiveChildMouseEvents(bool receive) { receive_child_mouse_events_ = receive; }
    /**
     * @brief Checks if this frame ignores mouse events.
     * @return True if the frame ignores mouse events, false otherwise.
     */
    bool ignoresMouseEvents() const { return ignores_mouse_events_; }
    /**
     * @brief Sets whether this frame ignores mouse events.
     * @param ignore True to ignore mouse events, false otherwise.
     * @param pass_to_children True to pass mouse events to children, false otherwise.
     */
    void setIgnoresMouseEvents(bool ignore, bool pass_to_children) {
      ignores_mouse_events_ = ignore;
      pass_mouse_events_to_children_ = pass_to_children;
    }
    /**
     * @brief Checks if this frame has keyboard focus.
     * @return True if the frame has keyboard focus, false otherwise.
     */
    bool hasKeyboardFocus() const { return keyboard_focus_; }
    /**
     * @brief Tries to focus a text receiver within this frame.
     * @return True if a text receiver was focused, false otherwise.
     */
    bool tryFocusTextReceiver();
    /**
     * @brief Focuses the next text receiver in the hierarchy.
     * @param starting_child The child to start searching from.
     * @return True if a text receiver was focused, false otherwise.
     */
    bool focusNextTextReceiver(const Frame* starting_child = nullptr) const;
    /**
     * @brief Focuses the previous text receiver in the hierarchy.
     * @param starting_child The child to start searching from.
     * @return True if a text receiver was focused, false otherwise.
     */
    bool focusPreviousTextReceiver(const Frame* starting_child = nullptr) const;
    /**
     * @brief Draws the frame to its region on the given canvas.
     * @param canvas The canvas to draw on.
     */
    void drawToRegion(Canvas& canvas);
    /**
     * @brief Sets the DPI scale for this frame and its children.
     * @param dpi_scale The new DPI scale.
     */
    void setDpiScale(float dpi_scale) {
      bool changed = dpi_scale_ != dpi_scale;
      dpi_scale_ = dpi_scale;

      if (changed) {
        on_dpi_change_.callback();
        redraw();
      }

      for (Frame* child : children_)
        child->setDpiScale(dpi_scale);
    }
    /**
     * @brief Gets the DPI scale of this frame.
     * @return The DPI scale.
     */
    float dpiScale() const { return dpi_scale_; }
    /**
     * @brief Requests a redraw of the frame through the event handler.
     * @return True if the request was sent, false otherwise.
     */
    bool requestRedraw() {
      if (event_handler_ && event_handler_->request_redraw) {
        event_handler_->request_redraw(this);
        return true;
      }
      return false;
    }
    /**
     * @brief Requests keyboard focus for the frame through the event handler.
     */
    void requestKeyboardFocus() {
      if (event_handler_ && event_handler_->request_keyboard_focus)
        event_handler_->request_keyboard_focus(this);
    }
    /**
     * @brief Notifies the frame and its children that they are being removed from the hierarchy.
     */
    void notifyRemoveFromHierarchy() {
      for (Frame* child : children_)
        child->notifyRemoveFromHierarchy();

      if (event_handler_ && event_handler_->remove_from_hierarchy)
        event_handler_->remove_from_hierarchy(this);
    }
    /**
     * @brief Sets the mouse relative mode through the event handler.
     * @param visible True to enable relative mode, false to disable it.
     */
    void setMouseRelativeMode(bool visible) {
      if (event_handler_ && event_handler_->set_mouse_relative_mode)
        event_handler_->set_mouse_relative_mode(visible);
    }
    /**
     * @brief Sets the cursor style through the event handler.
     * @param style The new cursor style.
     */
    void setCursorStyle(MouseCursor style) {
      if (event_handler_ && event_handler_->set_cursor_style)
        event_handler_->set_cursor_style(style);
    }
    /**
     * @brief Sets the cursor visibility through the event handler.
     * @param visible True to make the cursor visible, false to hide it.
     */
    void setCursorVisible(bool visible) {
      if (event_handler_ && event_handler_->set_cursor_visible)
        event_handler_->set_cursor_visible(visible);
    }
    /**
     * @brief Reads text from the clipboard through the event handler.
     * @return The text from the clipboard.
     */
    std::string readClipboardText() {
      if (event_handler_ && event_handler_->read_clipboard_text)
        return event_handler_->read_clipboard_text();
      return "";
    }
    /**
     * @brief Sets the clipboard text through the event handler.
     * @param text The text to set.
     */
    void setClipboardText(const std::string& text) {
      if (event_handler_ && event_handler_->set_clipboard_text)
        event_handler_->set_clipboard_text(text);
    }
    /**
     * @brief Processes a mouse enter event.
     * @param e The mouse event.
     */
    void processMouseEnter(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseEnter); }
    /**
     * @brief Processes a mouse exit event.
     * @param e The mouse event.
     */
    void processMouseExit(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseExit); }
    /**
     * @brief Processes a mouse down event.
     * @param e The mouse event.
     */
    void processMouseDown(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseDown); }
    /**
     * @brief Processes a mouse up event.
     * @param e The mouse event.
     */
    void processMouseUp(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseUp); }
    /**
     * @brief Processes a mouse move event.
     * @param e The mouse event.
     */
    void processMouseMove(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseMove); }
    /**
     * @brief Processes a mouse drag event.
     * @param e The mouse event.
     */
    void processMouseDrag(const MouseEvent& e) { propagateMouseEvent(e, &Frame::onMouseDrag); }
    /**
     * @brief Processes a mouse wheel event.
     * @param e The mouse event.
     * @return True if the event was handled, false otherwise.
     */
    bool processMouseWheel(const MouseEvent& e) { return on_mouse_wheel_.callback(e); }
    /**
     * @brief Processes a focus changed event.
     * @param is_focused True if the frame has gained focus, false otherwise.
     * @param was_clicked True if the focus change was due to a mouse click.
     */
    void processFocusChanged(bool is_focused, bool was_clicked) {
      keyboard_focus_ = is_focused && accepts_keystrokes_;
      focusChanged(is_focused, was_clicked);
    }
    /**
     * @brief Processes a key press event.
     * @param e The key event.
     * @return True if the event was handled, false otherwise.
     */
    bool processKeyPress(const KeyEvent& e) { return on_key_press_.callback(e); }
    /**
     * @brief Processes a key release event.
     * @param e The key event.
     * @return True if the event was handled, false otherwise.
     */
    bool processKeyRelease(const KeyEvent& e) { return on_key_release_.callback(e); }
    /**
     * @brief Processes a text input event.
     * @param text The text that was input.
     */
    void processTextInput(const std::string& text) { textInput(text); }
    /**
     * @brief Gets a value from the palette.
     * @param value_id The ID of the value to get.
     * @return The palette value.
     */
    float paletteValue(theme::ValueId value_id) const;
    /**
     * @brief Gets a color from the palette.
     * @param color_id The ID of the color to get.
     * @return The palette color as a Brush.
     */
    Brush paletteColor(theme::ColorId color_id) const;
    /**
     * @brief Adds an undoable action to the undo history.
     * @param action A unique_ptr to the UndoableAction.
     */
    void addUndoableAction(std::unique_ptr<UndoableAction> action) const;
    /**
     * @brief Triggers an undo operation.
     */
    void triggerUndo() const;
    /**
     * @brief Triggers a redo operation.
     */
    void triggerRedo() const;
    /**
     * @brief Checks if an undo operation can be performed.
     * @return True if undo is possible, false otherwise.
     */
    bool canUndo() const;
    /**
     * @brief Checks if a redo operation can be performed.
     * @return True if redo is possible, false otherwise.
     */
    bool canRedo() const;

  private:
    void propagateMouseEvent(const MouseEvent& e, void (Frame::*handler)(const MouseEvent&)) {
      (this->*handler)(e);
      auto frame = parent_;
      while (frame) {
        if (frame->receiveChildMouseEvents())
          (frame->*handler)(e);
        frame = frame->parent_;
      }
    }

    void onMouseEnter(const MouseEvent& e) { on_mouse_enter_.callback(e); }
    void onMouseExit(const MouseEvent& e) { on_mouse_exit_.callback(e); }
    void onMouseDown(const MouseEvent& e) { on_mouse_down_.callback(e); }
    void onMouseUp(const MouseEvent& e) { on_mouse_up_.callback(e); }
    void onMouseMove(const MouseEvent& e) { on_mouse_move_.callback(e); }
    void onMouseDrag(const MouseEvent& e) { on_mouse_drag_.callback(e); }

    void notifyHierarchyChanged() {
      for (Frame* child : children_)
        child->notifyHierarchyChanged();
      on_hierarchy_change_.callback();
    }

    void initChildren();
    void destroyChildren();
    void eraseChild(Frame* child);

    bool requiresLayer() const {
      return post_effect_ || backdrop_effect_ || cached_ || masked_ || alpha_transparency_ != 1.0f;
    }

    std::string name_;
    Bounds bounds_;
    IBounds native_bounds_;

    CallbackList<void(Canvas&)> on_draw_ { [this](Canvas& e) -> void { draw(e); } };
    CallbackList<void()> on_resize_ { [this] { resized(); } };
    CallbackList<void(const Frame*)> on_child_bounds_changed_;
    CallbackList<void(const Frame*)> on_child_added_;
    CallbackList<void(const Frame*)> on_child_removed_;
    CallbackList<void()> on_dpi_change_ { [this] { dpiChanged(); } };
    CallbackList<void()> on_visibility_change_ { [this] { visibilityChanged(); } };
    CallbackList<void()> on_hierarchy_change_ { [this] { hierarchyChanged(); } };
    CallbackList<void(bool, bool)> on_focus_change_ { [this](bool is_focused, bool was_clicked) {
      focusChanged(is_focused, was_clicked);
    } };
    CallbackList<void(const MouseEvent&)> on_mouse_enter_ { [this](auto& e) { mouseEnter(e); } };
    CallbackList<void(const MouseEvent&)> on_mouse_exit_ { [this](auto& e) { mouseExit(e); } };
    CallbackList<void(const MouseEvent&)> on_mouse_down_ { [this](auto& e) { mouseDown(e); } };
    CallbackList<void(const MouseEvent&)> on_mouse_up_ { [this](auto& e) { mouseUp(e); } };
    CallbackList<void(const MouseEvent&)> on_mouse_move_ { [this](auto& e) { mouseMove(e); } };
    CallbackList<void(const MouseEvent&)> on_mouse_drag_ { [this](auto& e) { mouseDrag(e); } };
    CallbackList<bool(const MouseEvent&)> on_mouse_wheel_ { [this](auto& e) { return mouseWheel(e); } };
    CallbackList<bool(const KeyEvent&)> on_key_press_ { [this](auto& e) { return keyPress(e); } };
    CallbackList<bool(const KeyEvent&)> on_key_release_ { [this](auto& e) { return keyRelease(e); } };
    CallbackList<void(const std::string&)> on_text_input_ { [this](auto& text) { textInput(text); } };

    bool on_top_ = false;
    bool visible_ = true;
    bool keyboard_focus_ = false;
    bool accepts_keystrokes_ = false;
    bool ignores_mouse_events_ = false;
    bool receive_child_mouse_events_ = false;
    bool pass_mouse_events_to_children_ = true;

    std::vector<Frame*> children_;
    std::map<Frame*, std::unique_ptr<Frame>> owned_children_;
    Frame* parent_ = nullptr;
    FrameEventHandler* event_handler_ = nullptr;

    float dpi_scale_ = 1.0f;
    Palette* palette_ = nullptr;
    theme::OverrideId palette_override_;
    bool initialized_ = false;

    PostEffect* post_effect_ = nullptr;
    std::unique_ptr<BlurPostEffect> blur_effect_;
    PostEffect* backdrop_effect_ = nullptr;
    bool cached_ = false;
    bool masked_ = false;
    float alpha_transparency_ = 1.0f;
    Region region_;
    std::unique_ptr<Layout> layout_;
    bool drawing_ = true;
    bool redrawing_ = false;
  };
}
