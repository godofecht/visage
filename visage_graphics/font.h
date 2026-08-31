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

#include "color.h"
#include "graphics_utils.h"
#include "visage_file_embed/embedded_file.h"

#include <map>
#include <vector>

namespace visage {
  class TypeFace;
  class PackedFont;
  /**
   * @struct PackedGlyph
   * @brief Represents a single glyph packed into a font atlas.
   */
  struct PackedGlyph {
    int atlas_left = -1;
    int atlas_top = -1;
    int width = -1;
    int height = -1;
    float x_offset = 0.0f;
    float y_offset = 0.0f;
    float x_advance = 0.0f;
    const TypeFace* type_face = nullptr;
  };
  /**
   * @struct FontAtlasQuad
   * @brief Represents a quad for rendering a glyph from a font atlas.
   */
  struct FontAtlasQuad {
    const PackedGlyph* packed_glyph;
    float x;
    float y;
    float width;
    float height;
  };
  /**
   * @class Font
   * @brief Represents a font for rendering text.
   *
   * This class manages the loading and rendering of a font, including its
   * associated glyphs and atlas texture.
   */
  class Font {
  public:
    static constexpr PackedGlyph kNullPackedGlyph = { 0, 0, 0, 0, 0.0f, 0.0f, 0.0f, nullptr };
    /**
     * @enum Justification
     * @brief Specifies the alignment of text.
     */
    enum Justification {
      kCenter = 0,
      kLeft = 0x1,
      kRight = 0x2,
      kTop = 0x10,
      kBottom = 0x20,
      kTopLeft = kTop | kLeft,
      kBottomLeft = kBottom | kLeft,
      kTopRight = kTop | kRight,
      kBottomRight = kBottom | kRight,
    };
    /**
     * @brief Checks if a character is a variation selector.
     * @param character The character to check.
     * @return True if the character is a variation selector, false otherwise.
     */
    static bool isVariationSelector(char32_t character) {
      return (character & 0xfffffff0) == 0xfe00;
    }
    /**
     * @brief Checks if a character is printable.
     * @param character The character to check.
     * @return True if the character is printable, false otherwise.
     */
    static bool isPrintable(char32_t character) {
      return character != ' ' && character != '\t' && character != '\n';
    }
    /**
     * @brief Checks if a character is a newline character.
     * @param character The character to check.
     * @return True if the character is a newline, false otherwise.
     */
    static bool isNewLine(char32_t character) { return character == '\n'; }
    /**
     * @brief Checks if a character should be ignored.
     * @param character The character to check.
     * @return True if the character should be ignored, false otherwise.
     */
    static bool isIgnored(char32_t character) {
      return character == '\r' || isVariationSelector(character);
    }
    /**
     * @brief Checks if a string contains a newline character.
     * @param string The string to check.
     * @param length The length of the string.
     * @return True if the string contains a newline, false otherwise.
     */
    static bool hasNewLine(const char32_t* string, int length);
    /**
     * @brief Default constructor.
     */
    Font() = default;
    /**
     * @brief Constructs a Font from raw data.
     * @param size The font size.
     * @param font_data A pointer to the font data.
     * @param data_size The size of the font data.
     * @param dpi_scale The DPI scale to use.
     */
    Font(float size, const unsigned char* font_data, int data_size, float dpi_scale = 0.0f);
    /**
     * @brief Constructs a Font from an embedded file.
     * @param size The font size.
     * @param file The embedded file containing the font data.
     * @param dpi_scale The DPI scale to use.
     */
    Font(float size, const EmbeddedFile& file, float dpi_scale = 0.0f);
    /**
     * @brief Constructs a Font from a file path.
     * @param size The font size.
     * @param file_path The path to the font file.
     * @param dpi_scale The DPI scale to use.
     */
    Font(float size, const std::string& file_path, float dpi_scale = 0.0f);
    Font(const Font& other);
    Font& operator=(const Font& other);
    ~Font();
    /**
     * @brief Gets the DPI scale of the font.
     * @return The DPI scale.
     */
    float dpiScale() const {
      // DPI scale must be set to get accurate measurements
      VISAGE_ASSERT(dpi_scale_);

      return dpi_scale_ ? dpi_scale_ : 1.0f;
    }
    /**
     * @brief Creates a new Font with a different DPI scale.
     * @param dpi_scale The new DPI scale.
     * @return The new Font object.
     */
    Font withDpiScale(float dpi_scale) const;
    /**
     * @brief Creates a new Font with a different size.
     * @param size The new size.
     * @return The new Font object.
     */
    Font withSize(float size) const;
    /**
     * @brief Finds the index of the character that overflows a given width.
     * @param string The string to measure.
     * @param string_length The length of the string.
     * @param width The width to check against.
     * @param round If true, round the width to the nearest integer.
     * @param character_override A character to use for all characters in the string.
     * @return The index of the overflowing character, or the string length if it fits.
     */
    int widthOverflowIndex(const char32_t* string, int string_length, float width,
                           bool round = false, int character_override = 0) const {
      return nativeWidthOverflowIndex(string, string_length, width * dpiScale(), round, character_override);
    }
    /**
     * @brief Calculates the line breaks for a string to fit within a given width.
     * @param string The string to measure.
     * @param length The length of the string.
     * @param width The width to fit within.
     * @return A vector of indices where line breaks should occur.
     */
    std::vector<int> lineBreaks(const char32_t* string, int length, float width) const {
      return nativeLineBreaks(string, length, width * dpiScale());
    }
    /**
     * @brief Calculates the width of a string.
     * @param string The string to measure.
     * @param length The length of the string.
     * @param character_override A character to use for all characters in the string.
     * @return The width of the string.
     */
    float stringWidth(const char32_t* string, int length, int character_override = 0) const {
      return nativeStringWidth(string, length, character_override) / dpiScale();
    }
    /**
     * @brief Calculates the width of a string.
     * @param string The string to measure.
     * @param character_override A character to use for all characters in the string.
     * @return The width of the string.
     */
    float stringWidth(const std::u32string& string, int character_override = 0) const {
      return stringWidth(string.c_str(), string.size(), character_override);
    }
    /**
     * @brief Gets the line height of the font.
     * @return The line height.
     */
    float lineHeight() const { return nativeLineHeight() / dpiScale(); }
    /**
     * @brief Gets the capital height of the font.
     * @return The capital height.
     */
    float capitalHeight() const { return nativeCapitalHeight() / dpiScale(); }
    /**
     * @brief Gets the lower dip height (descender) of the font.
     * @return The lower dip height.
     */
    float lowerDipHeight() const { return nativeLowerDipHeight() / dpiScale(); }

    /**
     * @brief Gets the width of the font atlas texture.
     * @return The atlas width.
     */
    int atlasWidth() const;
    /**
     * @brief Gets the height of the font atlas texture.
     * @return The atlas height.
     */
    int atlasHeight() const;
    /**
     * @brief Gets the size of the font.
     * @return The font size.
     */
    int size() const { return size_; }
    /**
     * @brief Gets the texture handle for the font atlas.
     * @return The texture handle.
     */
    const bgfx::TextureHandle& textureHandle() const;
    /**
     * @brief Sets the vertex positions for a string of text.
     * @param quads A pointer to an array of FontAtlasQuad objects to fill.
     * @param text The text to render.
     * @param length The length of the text.
     * @param x The x-coordinate of the text block.
     * @param y The y-coordinate of the text block.
     * @param width The width of the text block.
     * @param height The height of the text block.
     * @param justification The justification of the text.
     * @param character_override A character to use for all characters in the string.
     */
    void setVertexPositions(FontAtlasQuad* quads, const char32_t* text, int length, float x,
                            float y, float width, float height,
                            Justification justification = kCenter, int character_override = 0) const;
    /**
     * @brief Sets the vertex positions for a multi-line string of text.
     * @param quads A pointer to an array of FontAtlasQuad objects to fill.
     * @param text The text to render.
     * @param length The length of the text.
     * @param x The x-coordinate of the text block.
     * @param y The y-coordinate of the text block.
     * @param width The width of the text block.
     * @param height The height of the text block.
     * @param justification The justification of the text.
     */
    void setMultiLineVertexPositions(FontAtlasQuad* quads, const char32_t* text, int length,
                                     float x, float y, float width, float height,
                                     Justification justification = kCenter) const;
    /**
     * @brief Gets the underlying packed font data.
     * @return A pointer to the PackedFont.
     */
    const PackedFont* packedFont() const { return packed_font_; }

  private:
    int nativeWidthOverflowIndex(const char32_t* string, int string_length, float width,
                                 bool round = false, int character_override = 0) const;
    float nativeStringWidth(const char32_t* string, int length, int character_override = 0) const;
    int nativeLineHeight() const;
    float nativeCapitalHeight() const;
    float nativeLowerDipHeight() const;
    std::vector<int> nativeLineBreaks(const char32_t* string, int length, float width) const;

    float size_ = 0.0f;
    int native_size_ = 0;
    float dpi_scale_ = 0.0f;
    PackedFont* packed_font_ = nullptr;
  };
  /**
   * @class FontCache
   * @brief A cache for managing loaded fonts.
   *
   * This class is a singleton that manages the loading and unloading of fonts to
   * avoid redundant memory usage.
   */
  class FontCache {
  public:
    friend class Font;

    ~FontCache();
    /**
     * @brief Clears any stale fonts from the cache.
     */
    static void clearStaleFonts() {
      if (instance()->has_stale_fonts_)
        instance()->removeStaleFonts();
    }

  private:
    struct TypeFaceData {
      TypeFaceData(const unsigned char* data, int data_size) : data(data), data_size(data_size) { }
      const unsigned char* data = nullptr;
      int data_size = 0;

      bool operator<(const TypeFaceData& other) const {
        if (data_size != other.data_size)
          return data_size < other.data_size;
        if (data == nullptr || other.data == nullptr)
          return data < other.data;
        return std::memcmp(data, other.data, data_size) < 0;
      }
    };

    static FontCache* instance() {
      static FontCache cache;
      return &cache;
    }

    static PackedFont* loadPackedFont(int size, const EmbeddedFile& font) {
      std::string id = "embed: " + std::string(font.name) + " - " + std::to_string(size);
      return instance()->createOrLoadPackedFont(id, size, font.data, font.size);
    }

    static PackedFont* loadPackedFont(int size, const std::string& file_path);
    static PackedFont* loadPackedFont(const PackedFont* packed_font);
    static PackedFont* loadPackedFont(int size, const unsigned char* font_data, int data_size);

    static void returnPackedFont(PackedFont* packed_font) {
      instance()->decrementPackedFont(packed_font);
    }

    FontCache();

    PackedFont* incrementPackedFont(const std::string& id);
    PackedFont* createOrLoadPackedFont(const std::string& id, int size,
                                       const unsigned char* font_data, int data_size);
    void decrementPackedFont(PackedFont* packed_font);
    void removeStaleFonts();

    std::map<std::string, std::unique_ptr<PackedFont>> cache_;
    std::map<PackedFont*, int> ref_count_;
    std::map<TypeFaceData, std::unique_ptr<unsigned char[]>> type_face_data_lookup_;
    std::map<TypeFaceData, int> type_face_data_ref_count_;
    bool has_stale_fonts_ = false;
  };
}