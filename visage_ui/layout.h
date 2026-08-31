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

#include <functional>
#include <visage_utils/dimension.h>
#include <visage_utils/space.h>

namespace visage {
  /**
   * @class Layout
   * @brief Manages the layout of child frames using a flexbox-like model.
   */
  class Layout {
  public:
    /**
     * @enum ItemAlignment
     * @brief Specifies the alignment of items along the cross axis.
     */
    enum class ItemAlignment {
      NotSet,
      Stretch,
      Start,
      Center,
      End,
    };
    /**
     * @enum WrapAlignment
     * @brief Specifies the alignment of wrapped lines.
     */
    enum class WrapAlignment {
      Start,
      Center,
      End,
      Stretch,
      SpaceBetween,
      SpaceAround,
      SpaceEvenly
    };
    /**
     * @brief Calculates the positions of child layouts within a flex container.
     * @param children A vector of child layouts.
     * @param bounds The bounds of the container.
     * @param dpi_scale The DPI scale to use for calculations.
     * @return A vector of the calculated bounds for each child.
     */
    std::vector<IBounds> flexPositions(const std::vector<const Layout*>& children,
                                       const IBounds& bounds, float dpi_scale) {
      int pad_left = padding_before_[0].computeInt(dpi_scale, bounds.width(), bounds.height());
      int pad_right = padding_after_[0].computeInt(dpi_scale, bounds.width(), bounds.height());
      int pad_top = padding_before_[1].computeInt(dpi_scale, bounds.width(), bounds.height());
      int pad_bottom = padding_after_[1].computeInt(dpi_scale, bounds.width(), bounds.height());

      IBounds flex_bounds = { bounds.x() + pad_left, bounds.y() + pad_top,
                              bounds.width() - pad_left - pad_right,
                              bounds.height() - pad_top - pad_bottom };

      bounding_box_ = {};
      std::vector<IBounds> results;
      if (flex_wrap_)
        results = flexChildWrap(children, flex_bounds, dpi_scale, bounding_box_);
      else
        results = flexChildGroup(children, flex_bounds, dpi_scale, bounding_box_);

      bounding_box_.setX(bounding_box_.x() - pad_left);
      bounding_box_.setY(bounding_box_.y() - pad_top);
      bounding_box_.setWidth(bounding_box_.width() + pad_left + pad_right);
      bounding_box_.setHeight(bounding_box_.height() + pad_top + pad_bottom);
      return results;
    }
    /**
     * @brief Sets whether flex layout is enabled.
     * @param flex True to enable flex layout, false otherwise.
     */
    void setFlex(bool flex) { flex_ = flex; }
    /**
     * @brief Checks if flex layout is enabled.
     * @return True if flex layout is enabled, false otherwise.
     */
    bool flex() const { return flex_; }
    /**
     * @brief Sets the margin for all sides.
     * @param margin The margin value.
     */
    void setMargin(const Dimension& margin) {
      margin_before_[0] = margin;
      margin_before_[1] = margin;
      margin_after_[0] = margin;
      margin_after_[1] = margin;
    }
    /**
     * @brief Sets the left margin.
     * @param margin The margin value.
     */
    void setMarginLeft(const Dimension& margin) { margin_before_[0] = margin; }
    /**
     * @brief Sets the right margin.
     * @param margin The margin value.
     */
    void setMarginRight(const Dimension& margin) { margin_after_[0] = margin; }
    /**
     * @brief Sets the top margin.
     * @param margin The margin value.
     */
    void setMarginTop(const Dimension& margin) { margin_before_[1] = margin; }
    /**
     * @brief Sets the bottom margin.
     * @param margin The margin value.
     */
    void setMarginBottom(const Dimension& margin) { margin_after_[1] = margin; }
    const Dimension& marginLeft() { return margin_before_[0]; }
    const Dimension& marginRight() { return margin_after_[0]; }
    const Dimension& marginTop() { return margin_before_[1]; }
    const Dimension& marginBottom() { return margin_after_[1]; }
    /**
     * @brief Sets the padding for all sides.
     * @param padding The padding value.
     */
    void setPadding(const Dimension& padding) {
      padding_before_[0] = padding;
      padding_before_[1] = padding;
      padding_after_[0] = padding;
      padding_after_[1] = padding;
    }
    /**
     * @brief Sets the left padding.
     * @param padding The padding value.
     */
    void setPaddingLeft(const Dimension& padding) { padding_before_[0] = padding; }
    /**
     * @brief Sets the right padding.
     * @param padding The padding value.
     */
    void setPaddingRight(const Dimension& padding) { padding_after_[0] = padding; }
    /**
     * @brief Sets the top padding.
     * @param padding The padding value.
     */
    void setPaddingTop(const Dimension& padding) { padding_before_[1] = padding; }
    /**
     * @brief Sets the bottom padding.
     * @param padding The padding value.
     */
    void setPaddingBottom(const Dimension& padding) { padding_after_[1] = padding; }
    const Dimension& paddingLeft() { return padding_before_[0]; }
    const Dimension& paddingRight() { return padding_after_[0]; }
    const Dimension& paddingTop() { return padding_before_[1]; }
    const Dimension& paddingBottom() { return padding_after_[1]; }
    /**
     * @brief Sets the dimensions of the layout item.
     * @param width The width value.
     * @param height The height value.
     */
    void setDimensions(const Dimension& width, const Dimension& height) {
      dimensions_[0] = width;
      dimensions_[1] = height;
    }
    /**
     * @brief Sets the width of the layout item.
     * @param width The width value.
     */
    void setWidth(const Dimension& width) { dimensions_[0] = width; }
    /**
     * @brief Sets the height of the layout item.
     * @param height The height value.
     */
    void setHeight(const Dimension& height) { dimensions_[1] = height; }
    const Dimension& width() { return dimensions_[0]; }
    const Dimension& height() { return dimensions_[1]; }

    /**
     * @brief Sets the flex grow factor.
     * @param grow The flex grow factor.
     */
    void setFlexGrow(float grow) { flex_grow_ = grow; }
    /**
     * @brief Sets the flex shrink factor.
     * @param shrink The flex shrink factor.
     */
    void setFlexShrink(float shrink) { flex_shrink_ = shrink; }
    /**
     * @brief Sets the flex direction to rows or columns.
     * @param rows True for rows, false for columns.
     */
    void setFlexRows(bool rows) { flex_rows_ = rows; }
    /**
     * @brief Sets whether to reverse the flex direction.
     * @param reverse True to reverse the direction, false otherwise.
     */
    void setFlexReverseDirection(bool reverse) { flex_reverse_direction_ = reverse; }
    /**
     * @brief Sets whether to wrap flex items.
     * @param wrap True to enable wrapping, false otherwise.
     */
    void setFlexWrap(bool wrap) { flex_wrap_ = wrap ? 1 : 0; }
    /**
     * @brief Sets the alignment of items along the cross axis.
     * @param alignment The item alignment.
     */
    void setFlexItemAlignment(ItemAlignment alignment) { item_alignment_ = alignment; }
    /**
     * @brief Sets the alignment of this specific item along the cross axis.
     * @param alignment The self alignment.
     */
    void setFlexSelfAlignment(ItemAlignment alignment) { self_alignment_ = alignment; }
    /**
     * @brief Sets the alignment of wrapped lines.
     * @param alignment The wrap alignment.
     */
    void setFlexWrapAlignment(WrapAlignment alignment) { wrap_alignment_ = alignment; }
    /**
     * @brief Sets whether to reverse the wrap direction.
     * @param wrap True to reverse the wrap direction, false otherwise.
     */
    void setFlexWrapReverse(bool wrap) { flex_wrap_ = wrap ? -1 : 0; }
    /**
     * @brief Sets the gap between flex items.
     * @param gap The gap value.
     */
    void setFlexGap(Dimension gap) { flex_gap_ = std::move(gap); }
    /**
     * @brief Gets the bounding box of the layout.
     * @return The bounding box.
     */
    IBounds boundingBox() const { return bounding_box_; }

  private:
    std::vector<IBounds> flexChildGroup(const std::vector<const Layout*>& children, IBounds bounds,
                                        float dpi_scale, IBounds& bounding_box) const;

    std::vector<int> alignCrossPositions(std::vector<int>& cross_sizes, int cross_area, int gap) const;

    std::vector<IBounds> flexChildWrap(const std::vector<const Layout*>& children, IBounds bounds,
                                       float dpi_scale, IBounds& bounding_box) const;

    bool flex_ = false;
    IBounds bounding_box_;
    Dimension margin_before_[2];
    Dimension margin_after_[2];
    Dimension padding_before_[2];
    Dimension padding_after_[2];
    Dimension dimensions_[2];

    ItemAlignment item_alignment_ = ItemAlignment::Stretch;
    ItemAlignment self_alignment_ = ItemAlignment::NotSet;
    WrapAlignment wrap_alignment_ = WrapAlignment::Start;
    float flex_grow_ = 0.0f;
    float flex_shrink_ = 0.0f;
    bool flex_rows_ = true;
    bool flex_reverse_direction_ = false;
    int flex_wrap_ = 0;
    Dimension flex_gap_;
  };
}