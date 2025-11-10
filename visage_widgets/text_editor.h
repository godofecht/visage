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

#include "visage_graphics/font.h"
#include "visage_ui/frame.h"
#include "visage_ui/scroll_bar.h"

namespace visage {
  /**
   * @class TextEditor
   * @brief A widget for editing text.
   *
   * This class provides a multi-line text editor with support for selection,
   * clipboard operations, undo/redo, and password mode.
   */
  class TextEditor : public ScrollableFrame {
  public:
    /**
     * @brief The default character to use for password fields.
     */
    static constexpr int kDefaultPasswordCharacter = '*';
    /**
     * @brief The maximum number of undo history entries.
     */
    static constexpr int kMaxUndoHistory = 1000;

    static constexpr char32_t kAcuteAccentCharacter = U'\u00B4';
    static constexpr char32_t kGraveAccentCharacter = U'\u0060';
    static constexpr char32_t kTildeCharacter = U'\u02DC';
    static constexpr char32_t kUmlautCharacter = U'\u00A8';
    static constexpr char32_t kCircumflexCharacter = U'\u02C6';
    /**
     * @brief Checks if a character is alphanumeric.
     * @param character The character to check.
     * @return True if the character is alphanumeric, false otherwise.
     */
    static bool isAlphaNumeric(char character) {
      return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') ||
             (character >= '0' && character <= '9');
    }
    /**
     * @brief Checks if a character can be part of a variable name.
     * @param character The character to check.
     * @return True if the character is a valid variable character, false otherwise.
     */
    static bool isVariableCharacter(char character) {
      return isAlphaNumeric(character) || character == '_';
    }
    /**
     * @enum ActionState
     * @brief Represents the current action state of the text editor.
     */
    enum ActionState {
      kNone,
      kInserting,
      kDeleting,
    };
    /**
     * @enum DeadKey
     * @brief Represents a dead key for composing characters.
     */
    enum class DeadKey {
      None,
      AcuteAccent,
      GraveAccent,
      Tilde,
      Umlaut,
      Circumflex
    };

    VISAGE_THEME_DEFINE_COLOR(TextEditorBackground);
    VISAGE_THEME_DEFINE_COLOR(TextEditorBorder);
    VISAGE_THEME_DEFINE_COLOR(TextEditorText);
    VISAGE_THEME_DEFINE_COLOR(TextEditorDefaultText);
    VISAGE_THEME_DEFINE_COLOR(TextEditorCaret);
    VISAGE_THEME_DEFINE_COLOR(TextEditorSelection);

    VISAGE_THEME_DEFINE_VALUE(TextEditorRounding);
    VISAGE_THEME_DEFINE_VALUE(TextEditorMarginX);
    VISAGE_THEME_DEFINE_VALUE(TextEditorMarginY);
    /**
     * @brief Constructs a TextEditor.
     * @param name The name of the text editor.
     */
    explicit TextEditor(const std::string& name = "");
    ~TextEditor() override = default;
    /**
     * @brief Returns the callback list for the text change event.
     * @return A reference to the on_text_change_ callback list.
     */
    auto& onTextChange() { return on_text_change_; }
    /**
     * @brief Returns the callback list for the enter key press event.
     * @return A reference to the on_enter_key_ callback list.
     */
    auto& onEnterKey() { return on_enter_key_; }
    /**
     * @brief Returns the callback list for the escape key press event.
     * @return A reference to the on_escape_key_ callback list.
     */
    auto& onEscapeKey() { return on_escape_key_; }
    /**
     * @brief Draws the background of the text editor.
     * @param canvas The canvas to draw on.
     */
    void drawBackground(Canvas& canvas) const;
    /**
     * @brief Draws a selection rectangle.
     * @param canvas The canvas to draw on.
     * @param x The x-coordinate of the rectangle.
     * @param y The y-coordinate of the rectangle.
     * @param w The width of the rectangle.
     * @param h The height of the rectangle.
     */
    void selectionRectangle(Canvas& canvas, float x, float y, float w, float h) const;
    /**
     * @brief Draws the text selection.
     * @param canvas The canvas to draw on.
     */
    void drawSelection(Canvas& canvas) const;
    void draw(Canvas& canvas) override;
    /**
     * @brief Converts a character index to a pixel position.
     * @param index The character index.
     * @return The (x, y) pixel position.
     */
    std::pair<float, float> indexToPosition(int index) const;
    /**
     * @brief Gets the start and end character indices of a line.
     * @param line The line number.
     * @return A pair containing the start and end indices.
     */
    std::pair<int, int> lineRange(int line) const;
    /**
     * @brief Converts a pixel position to a character index.
     * @param position The (x, y) pixel position.
     * @return The character index.
     */
    int positionToIndex(const std::pair<float, float>& position) const;
    /**
     * @brief Cancels the current edit.
     */
    void cancel();
    /**
     * @brief Clears the current text selection.
     */
    void deselect();
    /**
     * @brief Clears all text from the editor.
     */
    void clear();
    /**
     * @brief Deletes the currently selected text.
     */
    void deleteSelected();
    /**
     * @brief Scrolls the view to make the caret visible.
     */
    void makeCaretVisible();
    /**
     * @brief Sets the view bounds for scrolling.
     */
    void setViewBounds();
    /**
     * @brief Gets the start index of the selection.
     * @return The selection start index.
     */
    int selectionStart() const { return std::min(caret_position_, selection_position_); }
    /**
     * @brief Gets the end index of the selection.
     * @return The selection end index.
     */
    int selectionEnd() const { return std::max(caret_position_, selection_position_); }
    /**
     * @brief Gets the currently selected text.
     * @return The selected text.
     */
    String selection() const;
    /**
     * @brief Gets the index of the beginning of the word at the caret.
     * @return The index of the beginning of the word.
     */
    int beginningOfWord() const;
    /**
     * @brief Gets the index of the end of the word at the caret.
     * @return The index of the end of the word.
     */
    int endOfWord() const;

    void resized() override {
      ScrollableFrame::resized();
      setBackgroundRounding(paletteValue(TextEditorRounding));
      setLineBreaks();
      makeCaretVisible();
    }

    void dpiChanged() override {
      Font f = font().withDpiScale(dpiScale());
      text_.setFont(f);
      default_text_.setFont(f);
    }

    void mouseEnter(const MouseEvent& e) override;
    void mouseExit(const MouseEvent& e) override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;
    /**
     * @brief Handles a double-click event.
     * @param e The mouse event.
     */
    void doubleClick(const MouseEvent& e);
    /**
     * @brief Handles a triple-click event.
     * @param e The mouse event.
     */
    void tripleClick(const MouseEvent& e);
    /**
     * @brief Handles a dead key press.
     * @param key The key event.
     * @return True if the key was handled, false otherwise.
     */
    bool handleDeadKey(const KeyEvent& key);
    bool keyPress(const KeyEvent& key) override;
    bool keyRelease(const KeyEvent& key) override;
    bool receivesTextInput() override { return active_; }
    /**
     * @brief Translates text containing a dead key character.
     * @param text The text to translate.
     * @return The translated text.
     */
    String translateDeadKeyText(const String& text) const;
    void textInput(const std::string& text) override;
    void focusChanged(bool is_focused, bool was_clicked) override;
    /**
     * @brief Moves the caret left.
     * @param modifier True if a modifier key is pressed.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretLeft(bool modifier, bool shift);
    /**
     * @brief Moves the caret right.
     * @param modifier True if a modifier key is pressed.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretRight(bool modifier, bool shift);
    /**
     * @brief Moves the caret vertically.
     * @param shift True if the shift key is pressed.
     * @param y_offset The vertical offset to move by.
     */
    void moveCaretVertically(bool shift, float y_offset);
    /**
     * @brief Handles an enter key press.
     * @return True if the event was handled, false otherwise.
     */
    bool enterPressed();
    /**
     * @brief Handles an escape key press.
     * @return True if the event was handled, false otherwise.
     */
    bool escapePressed();
    /**
     * @brief Moves the caret up one line.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretUp(bool shift);
    /**
     * @brief Moves the caret down one line.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretDown(bool shift);
    /**
     * @brief Moves the caret to the top of the document.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretToTop(bool shift);
    /**
     * @brief Moves the caret to the start of the current line.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretToStartOfLine(bool shift);
    /**
     * @brief Moves the caret to the end of the document.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretToEnd(bool shift);
    /**
     * @brief Moves the caret to the end of the current line.
     * @param shift True if the shift key is pressed.
     * @return True if the caret was moved, false otherwise.
     */
    bool moveCaretToEndOfLine(bool shift);
    /**
     * @brief Moves the view up by one page.
     * @param shift True if the shift key is pressed.
     * @return True if the view was moved, false otherwise.
     */
    bool pageUp(bool shift);
    /**
     * @brief Moves the view down by one page.
     * @param shift True if the shift key is pressed.
     * @return True if the view was moved, false otherwise.
     */
    bool pageDown(bool shift);
    /**
     * @brief Copies the selected text to the clipboard.
     * @return True if the operation was successful, false otherwise.
     */
    bool copyToClipboard();
    /**
     * @brief Cuts the selected text to the clipboard.
     * @return True if the operation was successful, false otherwise.
     */
    bool cutToClipboard();
    /**
     * @brief Pastes text from the clipboard.
     * @return True if the operation was successful, false otherwise.
     */
    bool pasteFromClipboard();
    /**
     * @brief Deletes text backwards from the caret.
     * @param modifier True if a modifier key is pressed.
     * @return True if text was deleted, false otherwise.
     */
    bool deleteBackwards(bool modifier);
    /**
     * @brief Deletes text forwards from the caret.
     * @param modifier True if a modifier key is pressed.
     * @return True if text was deleted, false otherwise.
     */
    bool deleteForwards(bool modifier);
    /**
     * @brief Selects all text in the editor.
     * @return True if the operation was successful, false otherwise.
     */
    bool selectAll();
    /**
     * @brief Undoes the last action.
     * @return True if the operation was successful, false otherwise.
     */
    bool undo();
    /**
     * @brief Redoes the last undone action.
     * @return True if the operation was successful, false otherwise.
     */
    bool redo();

    /**
     * @brief Inserts text at the current caret position.
     * @param insert_text The text to insert.
     */
    void insertTextAtCaret(const String& insert_text);
    /**
     * @brief Sets the rounding for the background.
     * @param rounding The new rounding value.
     */
    void setBackgroundRounding(float rounding) {
      background_rounding_ = rounding;
      setScrollBarRounding(rounding);
    }
    /**
     * @brief Sets the margin for the text.
     * @param x The horizontal margin.
     * @param y The vertical margin.
     */
    void setMargin(float x, float y) {
      set_x_margin_ = x;
      set_y_margin_ = y;
    }
    /**
     * @brief Gets the horizontal margin.
     * @return The horizontal margin.
     */
    float xMargin() const {
      if (text_.justification() & (Font::kLeft | Font::kRight))
        return xMarginSize();
      return 0;
    }
    /**
     * @brief Gets the vertical margin.
     * @return The vertical margin.
     */
    float yMargin() const {
      if (text_.justification() & Font::kTop)
        return set_y_margin_ ? set_y_margin_ : paletteValue(TextEditorMarginY);
      return 0;
    }
    /**
     * @brief Sets the text editor to password mode.
     * @param character The character to use for masking the password.
     */
    void setPassword(int character = kDefaultPasswordCharacter) {
      text_.setCharacterOverride(character);
      if (character) {
        setMultiLine(false);
        setJustification(Font::Justification::kLeft);
      }
    }
    /**
     * @brief Sets the line breaks for the text.
     */
    void setLineBreaks() {
      if (text_.multiLine() && text_.font().packedFont()) {
        line_breaks_ = text_.font().lineBreaks(text_.text().c_str(), text_.text().length(),
                                               width() - 2 * xMargin());
      }
    }
    /**
     * @brief Sets the text of the editor.
     * @param text The new text.
     */
    void setText(const String& text) {
      if (max_characters_)
        text_.setText(text.substring(0, max_characters_));
      else
        text_.setText(text);
      caret_position_ = text_.text().length();
      selection_position_ = caret_position_;
      setLineBreaks();
      makeCaretVisible();
    }
    /**
     * @brief Sets the characters that are allowed to be input.
     * @param characters A string of allowed characters.
     */
    void setFilteredCharacters(const std::string& characters) { filtered_characters_ = characters; }
    /**
     * @brief Sets the default text to display when the editor is empty.
     * @param default_text The default text.
     */
    void setDefaultText(const String& default_text) { default_text_.setText(default_text); }
    /**
     * @brief Sets the maximum number of characters allowed.
     * @param max The maximum number of characters.
     */
    void setMaxCharacters(int max) { max_characters_ = max; }
    /**
     * @brief Sets whether the editor is multi-line.
     * @param multi_line True for multi-line, false for single-line.
     */
    void setMultiLine(bool multi_line) {
      text_.setMultiLine(multi_line);
      default_text_.setMultiLine(multi_line);
      if (multi_line)
        x_position_ = 0;
    }
    /**
     * @brief Sets whether to select all text on focus.
     * @param select_on_focus True to select all on focus, false otherwise.
     */
    void setSelectOnFocus(bool select_on_focus) { select_on_focus_ = select_on_focus; }
    /**
     * @brief Sets the text justification.
     * @param justification The new justification.
     */
    void setJustification(Font::Justification justification) {
      text_.setJustification(justification);
      default_text_.setJustification(justification);
    }
    /**
     * @brief Sets the font for the text.
     * @param font The new font.
     */
    void setFont(const Font& font) {
      Font f = font.withDpiScale(dpiScale());
      text_.setFont(f);
      default_text_.setFont(f);
      setLineBreaks();
      makeCaretVisible();
    }
    /**
     * @brief Sets whether the editor is active.
     * @param active True to make the editor active, false otherwise.
     */
    void setActive(bool active) { active_ = active; }
    /**
     * @brief Configures the editor for number entry.
     */
    void setNumberEntry();
    /**
     * @brief Configures the editor for general text field entry.
     */
    void setTextFieldEntry();
    /**
     * @brief Gets the current text of the editor.
     * @return The current text.
     */
    const String& text() const { return text_.text(); }
    /**
     * @brief Gets the length of the text.
     * @return The text length.
     */
    int textLength() const { return text_.text().length(); }
    /**
     * @brief Gets the font used by the editor.
     * @return The font.
     */
    const Font& font() const { return text_.font(); }
    /**
     * @brief Gets the text justification.
     * @return The text justification.
     */
    Font::Justification justification() const { return text_.justification(); }
    /**
     * @brief Sets the background color ID from the theme.
     * @param color_id The new background color ID.
     */
    void setBackgroundColorId(theme::ColorId color_id) { background_color_id_ = color_id; }

  private:
    float xMarginSize() const {
      return set_x_margin_ ? set_x_margin_ : paletteValue(TextEditorMarginX);
    }
    void addUndoPosition() { undo_history_.emplace_back(text_.text(), caret_position_); }

    CallbackList<void()> on_text_change_;
    CallbackList<void()> on_enter_key_;
    CallbackList<void()> on_escape_key_;

    DeadKey dead_key_entry_ = DeadKey::None;
    Text text_;
    Text default_text_;
    std::string filtered_characters_;
    std::vector<int> line_breaks_;
    int caret_position_ = 0;
    int selection_position_ = 0;
    std::pair<float, float> selection_start_point_;
    std::pair<float, float> selection_end_point_;
    int max_characters_ = 0;

    bool select_on_focus_ = false;
    bool mouse_focus_ = false;
    bool active_ = true;

    theme::ColorId background_color_id_ = TextEditorBackground;
    float background_rounding_ = 1.0f;
    float set_x_margin_ = 0.0f;
    float set_y_margin_ = 0.0f;
    float x_position_ = 0.0f;

    ActionState action_state_ = kNone;
    std::vector<std::pair<String, int>> undo_history_;
    std::vector<std::pair<String, int>> undone_history_;

    VISAGE_LEAK_CHECKER(TextEditor)
  };
}