/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_RF_TIMESTAMP_H
#define SRSRAN_RF_TIMESTAMP_H

#include "srsran/interfaces/radio_interfaces.h"

#include <sstream>
#include <iomanip>

namespace srsran {

/**
 * Implemenation of the rf_buffer_interface for the current radio implementation which uses flat arrays.
 * @see rf_buffer_interface
 * @see radio
 *
 */
class rf_timestamp_t : public rf_timestamp_interface
{
public:
  /**
   * Default constructor, all timestamps are zero by default
   */
  rf_timestamp_t() = default;

  /**
   * Copy constructor
   * @param other the other object to copy
   */
  rf_timestamp_t(const rf_timestamp_t& other) { copy(other); }

  /**
   * Default destructor
   */
  ~rf_timestamp_t() = default;

  /**
   * Assignment operator
   */
  rf_timestamp_t& operator=(const rf_timestamp_t& other)
  {
    if (this != &other) {
      copy(other);
    }
    return *this;
  }

  /**
   * Move constructor
   */
  rf_timestamp_t(rf_timestamp_t&&) = delete;

  /**
   * Move assignment operator
   */
  rf_timestamp_t& operator=(rf_timestamp_t&&) = delete;

  /**
   * Gets a timestamp by reference
   * @return Given timestamp of the indicated device if the index is bounded, otherwise it returns the default timestamp
   */
  const srsran_timestamp_t& get(uint32_t idx) const override
  {
    if (idx >= SRSRAN_MAX_CHANNELS) {
      return default_ts;
    }

    return timestamps[idx];
  }

  /**
   * Get the timestamp array pointer
   * @return timestamp pointer with the value if the channel index is bounded. Otherwise, returns nullptr
   */
  srsran_timestamp_t* get_ptr(uint32_t idx) override
  {
    if (idx >= SRSRAN_MAX_CHANNELS) {
      return nullptr;
    }
    return &timestamps[idx];
  }

  /**
   * Add a given amount of seconds to all the timestamps
   * @param secs number of seconds
   */
  void add(double secs) override
  {
    for (srsran_timestamp_t& ts : timestamps) {
      srsran_timestamp_add(&ts, 0, secs);
    }
  }

  /**
   * Subtract a given amount of seconds to all the timestamps
   * @param secs number of seconds
   */
  void sub(double secs) override
  {
    for (srsran_timestamp_t& ts : timestamps) {
      srsran_timestamp_sub(&ts, 0, secs);
    }
  }

  // flawk - begin

  [[nodiscard]] static srsran_timestamp_t fix(const srsran_timestamp_t& ts)
  {
    const int64_t _full     = int64_t(ts.full_secs);
    const double  _frac     = double(ts.frac_secs);
    const int     _frac_int = int(_frac);
    int64_t       full_secs = _full + _frac_int;
    double        frac_secs = _frac - _frac_int;
    if (frac_secs < 0) {
      full_secs -= 1;
      frac_secs += 1;
    }
    return {(time_t)full_secs, frac_secs};
  }

  [[nodiscard]] static std::string tostring(const srsran_timestamp_t& ts)
  {
    std::stringstream frac;
    frac << std::fixed << std::setprecision(17) << std::abs(ts.frac_secs);
    std::stringstream res;
    res << (int64_t)ts.full_secs;
    res << frac.str().erase(0, 1);
    return res.str();
  }

  // flawk - end

private:
  const srsran_timestamp_t                            default_ts = {};
  std::array<srsran_timestamp_t, SRSRAN_MAX_CHANNELS> timestamps = {};
};
} // namespace srsran

#endif // SRSRAN_RF_TIMESTAMP_H
