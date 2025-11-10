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

#include "visage_file_embed/embedded_file.h"
#include "visage_graphics/animation.h"
#include "visage_graphics/svg.h"
#include "visage_graphics/text.h"
#include "visage_graphics/theme.h"
#include "visage_ui/frame.h"
#include "visage_ui/svg_frame.h"

#include <functional>

namespace visage {
  /**
   * @class Button
   * @brief A base class for button widgets.
   *
   * This class provides the basic functionality for a button, including handling
   * mouse events and managing hover state.
   */
  class Button : public Frame {
  public:
    /**
     * @brief Default constructor.
     */
    Button() { hover_amount_.setTargetValue(1.0f); }
    /**
     * @brief Constructs a Button with a given name.
     * @param name The name of the button.
     */
    explicit Button(const std::string& name) : Frame(name) { hover_amount_.setTargetValue(1.0f); }
    /**
     * @brief Returns the callback list for the toggle event.
     * @return A reference to the on_toggle_ callback list.
     */
    auto& onToggle() { return on_toggle_; }
    /**
     * @brief Toggles the state of the button.
     * @return True if the state was changed, false otherwise.
     */
    virtual bool toggle() { return false; }
    /**
     * @brief Sets the toggled state of the button.
     * @param toggled The new toggled state.
     */
    virtual void setToggled(bool toggled) { }
    /**
     * @brief Sets the toggled state of the button and notifies listeners.
     * @param toggled The new toggled state.
     */
    virtual void setToggledAndNotify(bool toggled) {
      if (toggled)
        notify(false);
    }
    /**
     * @brief Notifies listeners of a toggle event.
     * @param on The new toggle state.
     */
    void notify(bool on) { on_toggle_.callback(this, on); }
    /**
     * @brief Draws the button.
     * @param canvas The canvas to draw on.
     */
    void draw(Canvas& canvas) final;
    /**
     * @brief Draws the button with a specific hover amount.
     * @param canvas The canvas to draw on.
     * @param hover_amount The amount of hover to apply (0.0 to 1.0).
     */
    virtual void draw(Canvas& canvas, float hover_amount) { }
    /**
     * @brief Called when the mouse enters the button.
     * @param e The mouse event.
     */
    void mouseEnter(const MouseEvent& e) override;
    /**
     * @brief Called when the mouse exits the button.
     * @param e The mouse event.
     */
    void mouseExit(const MouseEvent& e) override;
    /**
     * @brief Called when a mouse button is pressed down on the button.
     * @param e The mouse event.
     */
    void mouseDown(const MouseEvent& e) override;
    /**
     * @brief Called when a mouse button is released on the button.
     * @param e The mouse event.
     */
    void mouseUp(const MouseEvent& e) override;
    /**
     * @brief Sets whether the button should toggle on mouse down.
     * @param mouse_down True to toggle on mouse down, false otherwise.
     */
    void setToggleOnMouseDown(bool mouse_down) { toggle_on_mouse_down_ = mouse_down; }
    /**
     * @brief Gets the current hover amount.
     * @return The hover amount.
     */
    float hoverAmount() const { return hover_amount_.value(); }
    /**
     * @brief Sets whether the button is active.
     * @param active True to make the button active, false otherwise.
     */
    void setActive(bool active) { active_ = active; }
    /**
     * @brief Checks if the button is active.
     * @return True if the button is active, false otherwise.
     */
    bool isActive() const { return active_; }
    /**
     * @brief Sets a function to be called to set up an undo action.
     * @param undo_setup_function The function to call.
     */
    void setUndoSetupFunction(std::function<void()> undo_setup_function) {
      undo_setup_function_ = std::move(undo_setup_function);
    }
    /**
     * @brief Gets the undo setup function.
     * @return The undo setup function.
     */
    std::function<void()> undoSetupFunction() { return undo_setup_function_; }
    /**
     * @brief Checks if the button was alt-clicked.
     * @return True if the button was alt-clicked, false otherwise.
     */
    bool wasAltClicked() const { return alt_clicked_; }

  private:
    CallbackList<void(Button*, bool)> on_toggle_;
    Animation<float> hover_amount_;
    std::function<void()> undo_setup_function_ = nullptr;

    bool active_ = true;
    bool toggle_on_mouse_down_ = false;
    bool set_pointer_cursor_ = true;
    bool alt_clicked_ = false;

    VISAGE_LEAK_CHECKER(Button)
  };
  /**
   * @class UiButton
   * @brief A standard UI button with text.
   */
  class UiButton : public Button {
  public:
    VISAGE_THEME_DEFINE_COLOR(UiButtonBackground);
    VISAGE_THEME_DEFINE_COLOR(UiButtonBackgroundHover);
    VISAGE_THEME_DEFINE_COLOR(UiButtonText);
    VISAGE_THEME_DEFINE_COLOR(UiButtonTextHover);

    VISAGE_THEME_DEFINE_COLOR(UiActionButtonBackground);
    VISAGE_THEME_DEFINE_COLOR(UiActionButtonBackgroundHover);
    VISAGE_THEME_DEFINE_COLOR(UiActionButtonText);
    VISAGE_THEME_DEFINE_COLOR(UiActionButtonTextHover);
    /**
     * @brief Constructs a UiButton with the given text.
     * @param text The text to display on the button.
     */
    explicit UiButton(const std::string& text);
    /**
     * @brief Default constructor.
     */
    UiButton() : UiButton("") { }
    /**
     * @brief Constructs a UiButton with the given text and font.
     * @param text The text to display on the button.
     * @param font The font to use for the text.
     */
    explicit UiButton(const std::string& text, const Font& font);
    /**
     * @brief Draws the background of the button.
     * @param canvas The canvas to draw on.
     * @param hover_amount The amount of hover to apply.
     */
    virtual void drawBackground(Canvas& canvas, float hover_amount);
    void draw(Canvas& canvas, float hover_amount) override;
    /**
     * @brief Sets the font for the button text.
     * @param font The new font.
     */
    void setFont(const Font& font) {
      text_.setFont(font);
      redraw();
    }
    /**
     * @brief Sets whether this button is an action button.
     * @param action True if this is an action button, false otherwise.
     */
    void setActionButton(bool action = true) {
      action_ = action;
      redraw();
    }
    /**
     * @brief Sets the text for the button.
     * @param text The new text.
     */
    void setText(const std::string& text) {
      text_.setText(text);
      redraw();
    }
    /**
     * @brief Sets whether to draw a border when the button is inactive.
     * @param border True to draw a border, false otherwise.
     */
    void drawBorderWhenInactive(bool border) { border_when_inactive_ = border; }

  private:
    Text text_;
    bool action_ = false;
    bool border_when_inactive_ = false;
  };
  /**
   * @class IconButton
   * @brief A button that displays an SVG icon.
   */
  class IconButton : public Button {
  public:
    /**
     * @brief The default shadow radius for the icon.
     */
    static constexpr float kDefaultShadowRadius = 3.0f;
    /**
     * @brief Constructs an IconButton.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    explicit IconButton(bool shadow = false) { initSettings(shadow); }
    /**
     * @brief Constructs an IconButton with the given icon.
     * @param icon The Svg object to display.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    explicit IconButton(const Svg& icon, bool shadow = false) {
      setIcon(icon);
      initSettings(shadow);
    }
    /**
     * @brief Constructs an IconButton with an icon from an embedded file.
     * @param icon_file The embedded file containing the SVG data.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    explicit IconButton(const EmbeddedFile& icon_file, bool shadow = false) {
      setIcon(icon_file);
      initSettings(shadow);
    }
    /**
     * @brief Constructs an IconButton with an icon from raw SVG data.
     * @param svg A pointer to the SVG data.
     * @param svg_size The size of the SVG data.
     */
    IconButton(const unsigned char* svg, int svg_size) { setIcon(svg, svg_size); }
    /**
     * @brief Sets the icon from an embedded file.
     * @param icon_file The embedded file containing the SVG data.
     */
    void setIcon(const EmbeddedFile& icon_file) { setIcon({ icon_file.data, icon_file.size }); }
    /**
     * @brief Sets the icon from raw SVG data.
     * @param svg A pointer to the SVG data.
     * @param svg_size The size of the SVG data.
     */
    void setIcon(const unsigned char* svg, int svg_size) { setIcon({ svg, svg_size }); }
    /**
     * @brief Sets the icon from an Svg object.
     * @param icon The Svg object to display.
     */
    void setIcon(const Svg& icon) {
      icon_.load(icon);
      shadow_.load(icon);
    }

    void draw(Canvas& canvas, float hover_amount) override;

    void resized() override {
      Button::resized();
      icon_.setBounds(localBounds());
      shadow_.setBounds(localBounds());
    }
    /**
     * @brief Sets the shadow radius for the icon.
     * @param radius The new shadow radius.
     */
    void setShadowRadius(const Dimension& radius) {
      shadow_radius_ = radius;
      computeShadowRadius();
    }
    /**
     * @brief Sets the margin for the icon.
     * @param margin The new margin.
     */
    void setMargin(const Dimension& margin) {
      icon_.setMargin(margin);
      shadow_.setMargin(margin);
    }

  private:
    void initSettings(bool shadow) {
      addChild(shadow_, shadow);
      shadow_.setIgnoresMouseEvents(true, false);

      addChild(icon_);
      icon_.setIgnoresMouseEvents(true, false);
      if (shadow)
        setShadowRadius(kDefaultShadowRadius);
    }

    void computeShadowRadius() {
      float r = shadow_radius_.compute(dpiScale(), nativeWidth(), nativeHeight(), 0.0f) / dpiScale();
      shadow_.setVisible(r > 0.0f);
      shadow_.setBlurRadius(r);
    }

    SvgFrame icon_;
    SvgFrame shadow_;

    Dimension shadow_radius_;
  };

  class ButtonChangeAction;
  /**
   * @class ToggleButton
   * @brief A button that can be toggled on and off.
   */
  class ToggleButton : public Button {
  public:
    VISAGE_THEME_DEFINE_COLOR(ToggleButtonDisabled);
    VISAGE_THEME_DEFINE_COLOR(ToggleButtonOff);
    VISAGE_THEME_DEFINE_COLOR(ToggleButtonOffHover);
    VISAGE_THEME_DEFINE_COLOR(ToggleButtonOn);
    VISAGE_THEME_DEFINE_COLOR(ToggleButtonOnHover);
    /**
     * @brief Default constructor.
     */
    ToggleButton() = default;
    /**
     * @brief Constructs a ToggleButton with the given name.
     * @param name The name of the button.
     */
    explicit ToggleButton(const std::string& name) : Button(name) { }

    bool toggle() override;
    void setToggled(bool toggled) override {
      toggled_ = toggled;
      toggleValueChanged();
      redraw();
    }
    /**
     * @brief Called when the toggle value changes.
     */
    virtual void toggleValueChanged() { }
    void setToggledAndNotify(bool toggled) override {
      toggled_ = toggled;
      redraw();
      notify(toggled);
    }
    /**
     * @brief Checks if the button is toggled.
     * @return True if the button is toggled, false otherwise.
     */
    bool toggled() const { return toggled_; }
    /**
     * @brief Sets whether the button's action is undoable.
     * @param undoable True if the action is undoable, false otherwise.
     */
    void setUndoable(bool undoable) { undoable_ = undoable; }

  private:
    bool toggled_ = false;
    bool undoable_ = true;

    VISAGE_LEAK_CHECKER(ToggleButton)
  };
  /**
   * @class ButtonChangeAction
   * @brief An undoable action for a toggle button change.
   */
  class ButtonChangeAction : public UndoableAction {
  public:
    /**
     * @brief Constructs a ButtonChangeAction.
     * @param button A pointer to the ToggleButton.
     * @param toggled_on The new toggled state.
     */
    ButtonChangeAction(ToggleButton* button, bool toggled_on) :
        button_(button), toggled_on_(toggled_on) { }

    void undo() override { button_->setToggledAndNotify(!toggled_on_); }
    void redo() override { button_->setToggledAndNotify(toggled_on_); }

  private:
    ToggleButton* button_ = nullptr;
    bool toggled_on_ = false;
  };
  /**
   * @class ToggleIconButton
   * @brief A toggle button that displays an SVG icon.
   */
  class ToggleIconButton : public ToggleButton {
  public:
    /**
     * @brief The default shadow radius for the icon.
     */
    static constexpr float kDefaultShadowRadius = 3.0f;
    /**
     * @brief Constructs a ToggleIconButton with the given icon.
     * @param icon The Svg object to display.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    explicit ToggleIconButton(const Svg& icon, bool shadow = false) : ToggleButton() {
      setIcon(icon);
      initSettings(shadow);
    }
    /**
     * @brief Constructs a ToggleIconButton with the given name and icon.
     * @param name The name of the button.
     * @param icon The Svg object to display.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    ToggleIconButton(const std::string& name, const Svg& icon, bool shadow = false) :
        ToggleButton(name) {
      setIcon(icon);
      initSettings(shadow);
    }
    /**
     * @brief Constructs a ToggleIconButton with an icon from raw SVG data.
     * @param svg A pointer to the SVG data.
     * @param svg_size The size of the SVG data.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    ToggleIconButton(const unsigned char* svg, int svg_size, bool shadow = false) : ToggleButton() {
      setIcon({ svg, svg_size });
      initSettings(shadow);
    }
    /**
     * @brief Constructs a ToggleIconButton with a name and an icon from raw SVG data.
     * @param name The name of the button.
     * @param svg A pointer to the SVG data.
     * @param svg_size The size of the SVG data.
     * @param shadow If true, a shadow will be displayed behind the icon.
     */
    ToggleIconButton(const std::string& name, const unsigned char* svg, int svg_size,
                     bool shadow = false) : ToggleButton(name) {
      setIcon({ svg, svg_size });
      initSettings(shadow);
    }
    /**
     * @brief Sets the icon from an Svg object.
     * @param icon The Svg object to display.
     */
    void setIcon(const Svg& icon) {
      shadow_.load(icon);
      icon_.load(icon);
    }

    void draw(Canvas& canvas, float hover_amount) override;
    void resized() override;
    /**
     * @brief Sets the shadow radius for the icon.
     * @param radius The new shadow radius.
     */
    void setShadowRadius(const Dimension& radius) {
      shadow_radius_ = radius;
      computeShadowRadius();
    }
    /**
     * @brief Sets the margin for the icon.
     * @param margin The new margin.
     */
    void setMargin(const Dimension& margin) {
      icon_.setMargin(margin);
      shadow_.setMargin(margin);
    }

  private:
    void initSettings(bool shadow) {
      addChild(shadow_, shadow);
      shadow_.setIgnoresMouseEvents(true, false);

      addChild(icon_);
      icon_.setIgnoresMouseEvents(true, false);
      if (shadow)
        setShadowRadius(kDefaultShadowRadius);
    }

    void computeShadowRadius() {
      float r = shadow_radius_.compute(dpiScale(), nativeWidth(), nativeHeight(), 0.0f) / dpiScale();
      shadow_.setVisible(r > 0.0f);
      shadow_.setBlurRadius(r);
    }

    SvgFrame icon_;
    SvgFrame shadow_;

    Dimension shadow_radius_;
    Dimension margin_;
  };
  /**
   * @class ToggleTextButton
   * @brief A toggle button that displays text.
   */
  class ToggleTextButton : public ToggleButton {
  public:
    /**
     * @brief Constructs a ToggleTextButton with the given name.
     * @param name The name of the button, which is also used as the text.
     */
    explicit ToggleTextButton(const std::string& name);
    /**
     * @brief Constructs a ToggleTextButton with the given name and font.
     * @param name The name of the button, which is also used as the text.
     * @param font The font to use for the text.
     */
    explicit ToggleTextButton(const std::string& name, const Font& font);
    /**
     * @brief Draws the background of the button.
     * @param canvas The canvas to draw on.
     * @param hover_amount The amount of hover to apply.
     */
    virtual void drawBackground(Canvas& canvas, float hover_amount);
    void draw(Canvas& canvas, float hover_amount) override;
    /**
     * @brief Sets the font for the button text.
     * @param font The new font.
     */
    void setFont(const Font& font) {
      text_.setFont(font);
      redraw();
    }
    /**
     * @brief Sets the text for the button.
     * @param text The new text.
     */
    void setText(const std::string& text) { text_.setText(text); }
    /**
     * @brief Sets whether to draw the background of the button.
     * @param draw_background True to draw the background, false otherwise.
     */
    void setDrawBackground(bool draw_background) { draw_background_ = draw_background; }

  private:
    bool draw_background_ = true;
    Text text_;
  };
}
