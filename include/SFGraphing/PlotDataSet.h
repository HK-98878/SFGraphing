#pragma once
/**
 * @file Plot data file
 *
 * @author Jerrit Gläsker
 * @date 22.05.202
 *
 * @license MIT-License
 */

#include <vector>
#include <string>
#include <iostream>
#include "SFML/Graphics.hpp"

namespace csrc {
enum PlottingType { POINTS, LINE, BARS };

template <typename T, typename = void>
struct is_iterable_test : std::false_type {};
template <typename T>
struct is_iterable_test<
  T, std::void_t<decltype(std::begin(std::declval<T>())), decltype(std::end(std::declval<T>()))>>
    : std::true_type {};
template <typename T>
constexpr bool is_iterable = is_iterable_test<T>::value;

class PlotDataSet {
  private:
  std::vector<float> _yValues;
  std::vector<float> _xValues;
  sf::Color _color;
  std::string _label;
  PlottingType _pType;

  public:
  /**
   * Create empty data
   */
  PlotDataSet();

  /**
   * Constructor for empty data, but initializing other properties
   * @param color dataset color
   * @param label dataset label
   * @param type dataset type
   */
  PlotDataSet(const sf::Color& color, const std::string& label, const PlottingType& type);

  /**
   * Dataset constructor
   * @param xValues vector of doubles, representing x axis
   * @param yValues vector of doubles, representing y axis
   * @param color   color of the bars/points/line
   * @param label   data label
   * @param type    bars/points/line
   */
  template <class T_x, class T_y>
  PlotDataSet(const std::vector<T_x>& xValues, const std::vector<T_y>& yValues,
              const sf::Color& color, const std::string& label, const PlottingType& type)
      : _color(color), _label(label), _pType(type) {
    _xValues = std::vector<float>(xValues.begin(), xValues.end());
    _yValues = std::vector<float>(yValues.begin(), yValues.end());

    if (_xValues.size() != _yValues.size()) {
      std::cerr << "Incompatible data sizes" << std::endl;
      exit(1);
    }
  }

  template <class T_x, class T_y>
  static std::vector<PlotDataSet> MultiPlotDatSet(const std::vector<T_x>& xValues,
                                                  const std::vector<T_y>& yValues,
                                                  const sf::Color& color, const PlottingType& type,
                                                  const std::vector<std::string>& labels = {}) {
    if constexpr (!is_iterable<T_y> && !is_iterable<T_x>) {  // Only one dataset
      return std::vector<PlotDataSet>{
        PlotDataSet(xValues, yValues, color, labels.empty() ? "" : labels[0], type)};
    }
    if constexpr (is_iterable<T_y> && !is_iterable<T_x>) {  // Several Y values for each X value
      size_t sz = xValues.size();
      size_t internal_sz = yValues[0].size();
      if (sz == 0)
        throw std::runtime_error("Empty y value data");

      size_t labels_sz = labels.size();
      std::vector<PlotDataSet> out(sz);

      std::vector<std::vector<float>> data = std::vector<std::vector<float>>(internal_sz);
      for (size_t i = 0; i < internal_sz; ++i) {
        data[i] = std::vector<float>(sz);
        for (size_t j = 0; j < sz; ++j) {
          data[i][j] = yValues[j][i];
        }
      }

      for (int i = 0; i < internal_sz; ++i) {
        out[i] = PlotDataSet(xValues, data[i], color, i < labels_sz ? labels[i] : "", type);
      }
      return out;
    }
    if constexpr (is_iterable<T_x> && is_iterable<T_y>) {  // Several X and Y values
      size_t sz = xValues.size();
      size_t internal_sz = yValues[0].size();
      if (yValues.size() != sz) {
        throw std::runtime_error("Mismatching dataset sizes for mutliplot, set dimension of X"
                                 "must be one, or match set dimension of Y");
      }

      int labels_sz = labels.size();
      std::vector<PlotDataSet> out(sz);

      std::vector<std::vector<float>> xdata = std::vector<std::vector<float>>(internal_sz);
      std::vector<std::vector<float>> ydata = std::vector<std::vector<float>>(internal_sz);
      for (size_t i = 0; i < internal_sz; ++i) {
        xdata[i] = std::vector<float>(sz);
        ydata[i] = std::vector<float>(sz);
        for (size_t j = 0; j < sz; ++j) {
          xdata[i][j] = xValues[j][i];
          ydata[i][j] = yValues[j][i];
        }
      }

      for (int i = 0; i < sz; ++i) {
        out[i] = PlotDataSet(xdata[i], ydata[i], color, i < labels_sz ? labels[i] : "", type);
      }
      return out;
    }
    return {};
  }

  /**
   * _xValues Getter
   * @return _xValues
   */
  std::vector<float> GetXValues() const;

  /**
   * _yValues Getter
   * @return _yValues
   */
  std::vector<float> GetYValues() const;

  /**
   * Get element count (_xValues ad _yValues always the same size)
   * @return _xValues.size()
   */
  size_t GetDataLength() const;

  /**
   * Get a specific value
   * @param  i index
   * @return   value pair at i
   */
  sf::Vector2f GetDataValue(const size_t& i) const;

  /**
   * Setter for a certain value, by index
   * @param i         index
   * @param valuePair new x and y values
   */
  void SetDataValue(const size_t& i, const sf::Vector2f& valuePair);

  /**
   * _color Getter
   * @return _color
   */
  sf::Color GetColor() const;

  /**
   * _color Setter
   * @param color _color
   */
  void SetColor(const sf::Color& color);

  /**
   * _label Getter
   * @return _label
   */
  std::string GetLabel() const;

  /**
   * _label Setter
   * @param lab new _label
   */
  void SetLabel(const std::string& lab);

  /**
   * _plotType Getter
   * @return _plotType
   */
  PlottingType GetPlottingType() const;

  /**
   * _plotType Setter
   * @param type new _plotType
   */
  void SetPlottingType(const PlottingType& type);

  /**
   * Function for adding a pair of values to the dataset
   * @param pair the pair to be added
   */
  void PushPair(const sf::Vector2f& pair);

  /**
   * Function for erasing a pair of values to the dataset
   * @param number of elements to erase
   */
  void PopFront(const size_t n = 1);
};
}  // namespace csrc
