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
#include "frame.h"
#include "scroll_bar.h"
#include "visage_graphics/font.h"

#include <climits>

namespace visage {
  class PopupMenu;
#if defined(__APPLE__) && defined(__MACH__)
  void setNativeMenuBar(const PopupMenu& menu);
#else
  inline void setNativeMenuBar(const PopupMenu& menu) { }
#endif
  /**
   * @class PopupMenu
   * @brief Represents a popup menu with options and sub-menus.
   */
  class PopupMenu {
  public:
    static constexpr int kNotSet = INT_MIN;
    /**
     * @brief Default constructor.
     */
    PopupMenu() = default;
    /**
     * @brief Constructs a PopupMenu.
     * @param name The name of the menu item.
     * @param id The ID of the menu item.
     * @param options A vector of sub-menu options.
     * @param is_break If true, this item is a separator.
     */
    PopupMenu(const String& name, int id = -1, std::vector<PopupMenu> options = {}, bool is_break = false) :
        name_(name), id_(id), is_break_(is_break), options_(std::move(options)) { }
    /**
     * @brief Shows the popup menu.
     * @param source The frame that is the source of the menu.
     * @param position The position to show the menu at.
     */
    void show(Frame* source, Point position = { kNotSet, kNotSet });
    /**
     * @brief Sets this menu as the native menu bar (macOS only).
     */
    void setAsNativeMenuBar() { setNativeMenuBar(*this); }
    /**
     * @brief Adds an option to the menu.
     * @param option_id The ID of the option.
     * @param option_name The name of the option.
     * @return A reference to the newly added PopupMenu item.
     */
    PopupMenu& addOption(int option_id, const String& option_name) {
      options_.emplace_back(option_name, option_id);
      return options_.back();
    }
    /**
     * @brief Sets the selected state of the menu item.
     * @param selected The new selected state.
     * @return A reference to this PopupMenu.
     */
    PopupMenu& select(bool selected) {
      selected_ = selected;
      return *this;
    }

    bool selected() const { return selected_; }
    /**
     * @brief Sets the enabled state of the menu item.
     * @param enabled The new enabled state.
     * @return A reference to this PopupMenu.
     */
    PopupMenu& enable(bool enabled) {
      enabled_ = enabled;
      return *this;
    }
    /**
     * @brief Sets a native keyboard shortcut for the menu item.
     * @param modifiers The modifier keys.
     * @param character The character key.
     * @return A reference to this PopupMenu.
     */
    PopupMenu& withNativeKeyboardShortcut(int modifiers, std::string character) {
      shortcut_modifiers_ = modifiers;
      shortcut_character_ = character;
      return *this;
    }

    const int nativeShortcutModifiers() const { return shortcut_modifiers_; }
    const std::string& nativeShortcutCharacter() const { return shortcut_character_; }

    bool enabled() const { return enabled_; }
    /**
     * @brief Returns the callback list for the selection event.
     * @return A reference to the on_selection_ callback list.
     */
    auto& onSelection() { return on_selection_; }
    /**
     * @brief Returns the callback list for the cancel event.
     * @return A reference to the on_cancel_ callback list.
     */
    auto& onCancel() { return on_cancel_; }

    const auto& onSelection() const { return on_selection_; }
    const auto& onCancel() const { return on_cancel_; }
    /**
     * @brief Adds a sub-menu to this menu.
     * @param sub_menu The sub-menu to add.
     */
    void addSubMenu(PopupMenu sub_menu) { options_.push_back(std::move(sub_menu)); }
    /**
     * @brief Adds a separator to the menu.
     */
    void addBreak() { options_.push_back({ "", -1, {}, true }); }

    const std::vector<PopupMenu>& options() const { return options_; }
    int size() const { return options_.size(); }

    int id() const { return id_; }
    const String& name() const { return name_; }
    bool isBreak() const { return is_break_; }
    bool hasOptions() const { return !options_.empty(); }

  private:
    CallbackList<void(int)> on_selection_;
    CallbackList<void()> on_cancel_;
    String name_;
    int id_ = -1;
    bool is_break_ = false;
    bool selected_ = false;
    bool enabled_ = true;

    int shortcut_modifiers_ = 0;
    std::string shortcut_character_;
    std::vector<PopupMenu> options_;
  };
  /**
   * @class PopupList
   * @brief A scrollable list of popup menu options.
   */
  class PopupList : public ScrollableFrame {
  public:
    /**
     * @class Listener
     * @brief An interface for responding to events from a PopupList.
     */
    class Listener {
    public:
      virtual ~Listener() = default;
      virtual void optionSelected(const PopupMenu& option, PopupList* list) = 0;
      virtual void subMenuSelected(const PopupMenu& option, int selected_y, PopupList* list) = 0;
      virtual void mouseMovedOnMenu(Point position, PopupList* list) = 0;
      virtual void mouseDraggedOnMenu(Point position, PopupList* list) = 0;
      virtual void mouseUpOutside(Point position, PopupList* list) = 0;
    };

    PopupList() = default;
    /**
     * @brief Sets the options to be displayed in the list.
     * @param options A vector of PopupMenu objects.
     */
    void setOptions(std::vector<PopupMenu> options) { options_ = std::move(options); }
    /**
     * @brief Sets the font to be used for the text in the list.
     * @param font The font to use.
     */
    void setFont(const Font& font) { font_ = font.withDpiScale(dpiScale()); }

    float renderHeight() const;
    float renderWidth() const;

    int yForIndex(int index) const;
    int hoverY() { return yForIndex(hover_index_); }
    int hoverIndex() const { return hover_index_; }
    int numOptions() const { return options_.size(); }
    const PopupMenu& option(int index) const { return options_[index]; }
    void selectHoveredIndex();
    void setHoverFromPosition(Point position);
    void setNoHover() { hover_index_ = -1; }
    void selectFromPosition(Point position);

    void draw(Canvas& canvas) override;
    void resized() override;

    void enableMouseUp(bool enable) { enable_mouse_up_ = enable; }

    void mouseExit(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseMove(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    bool mouseWheel(const MouseEvent& e) override {
      bool result = ScrollableFrame::mouseWheel(e);
      if (!isVisible())
        return result;

      for (Listener* listener : listeners_)
        listener->mouseMovedOnMenu(e.relativeTo(this).position, this);
      return result;
    }
    /**
     * @brief Adds a listener to this PopupList.
     * @param listener The listener to add.
     */
    void addListener(Listener* listener) { listeners_.push_back(listener); }
    /**
     * @brief Resets the currently open sub-menu.
     */
    void resetOpenMenu() { menu_open_index_ = -1; }
    /**
     * @brief Sets the currently open sub-menu.
     * @param index The index of the sub-menu to open.
     */
    void setOpenMenu(int index) { menu_open_index_ = index; }
    /**
     * @brief Sets the opacity of the list.
     * @param opacity The new opacity value.
     */
    void setOpacity(float opacity) {
      opacity_ = opacity;
      redraw();
    }

  private:
    std::vector<Listener*> listeners_;
    std::vector<PopupMenu> options_;
    float opacity_ = 0.0f;
    int hover_index_ = -1;
    int menu_open_index_ = -1;
    bool enable_mouse_up_ = false;
    Font font_;
  };

  class PopupMenuFrame : public Frame,
                         public PopupList::Listener,
                         public EventTimer {
  public:
    static constexpr int kMaxSubMenus = 4;
    static constexpr int kWaitForSelection = 20;
    static constexpr int kPauseMs = 400;

    PopupMenuFrame(PopupMenu menu);
    ~PopupMenuFrame() override;

    void draw(Canvas& canvas) override;

    void show(std::unique_ptr<PopupMenuFrame> self, Frame* source, Point point = {});
    void setFont(const Font& font) {
      font_ = font;
      setListFonts(font);
    }
    void setListFonts(const Font& font) {
      for (auto& list : lists_)
        list.setFont(font);
    }

    void exit();
    void hierarchyChanged() override;
    void focusChanged(bool is_focused, bool was_clicked) override;
    void visibilityChanged() override { opacity_animation_.target(isVisible(), true); }
    void timerCallback() override;

    void optionSelected(const PopupMenu& option, PopupList* list) override;
    void subMenuSelected(const PopupMenu& option, int selection_y, PopupList* list) override;
    void mouseMovedOnMenu(Point position, PopupList* list) override { moveHover(position, list); }
    void mouseDraggedOnMenu(Point position, PopupList* list) override { moveHover(position, list); }
    void mouseUpOutside(Point position, PopupList* list) override;
    void moveHover(Point position, const PopupList* list);
    float opacity() const { return opacity_animation_.value(); }

  private:
    PopupMenu menu_;
    Frame* parent_ = nullptr;
    bool done_ = false;
    Animation<float> opacity_animation_;
    PopupList lists_[kMaxSubMenus];
    int hover_index_ = -1;
    Font font_;
    PopupList* hover_list_ = nullptr;

    VISAGE_LEAK_CHECKER(PopupMenuFrame)
  };

  class ValueDisplay : public Frame {
  public:
    ValueDisplay() { setIgnoresMouseEvents(true, false); }

    void draw(Canvas& canvas) override;

    void showDisplay(const String& text, Bounds bounds, Font::Justification justification);
    void setFont(const Font& font) { font_ = font; }

  private:
    Font font_;
    String text_;

    VISAGE_LEAK_CHECKER(ValueDisplay)
  };
}