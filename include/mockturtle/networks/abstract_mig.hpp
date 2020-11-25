/* mockturtle: C++ logic network library
 * Copyright (C) 2018-2019  EPFL
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <memory>
#include <optional>
#include <string>

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>

#include "../traits.hpp"
#include "../utils/algorithm.hpp"
#include "detail/foreach.hpp"

namespace mockturtle
{

class abstract_mig_network
{
public:
#pragma region Types and constructors
  static constexpr auto min_fanin_size = /* TODO */;
  static constexpr auto max_fanin_size = std::numeric_limits<uint32_t>::max();

  using base_type = abstract_mig_network;
  using node = uint32_t;

  struct signal
  {
    // TODO

    signal operator!() const
    { /* TODO */ }

    signal operator+() const
    { /* TODO */ }

    signal operator-() const
    { /* TODO */ }

    signal operator^( bool complement ) const
    { /* TODO */ }

    bool operator==( signal const& other ) const
    { /* TODO */ }

    bool operator!=( signal const& other ) const
    { /* TODO */ }

    bool operator<( signal const& other ) const
    { /* TODO */ }
  };

  struct storage_type
  {
    // TODO
  };
  using storage = std::shared_ptr<storage_type>;

  abstract_mig_network()
  {
    // TODO
  }
#pragma endregion

#pragma region Primary I / O and constants
  signal get_constant( bool value ) const
  {
    // TODO
  }

  signal create_pi( std::string const& name = std::string() )
  {
    // TODO
  }

  uint32_t create_po( signal const& f, std::string const& name = std::string() )
  {
    // TODO
  }

  bool is_pi( node const& n ) const
  {
    // TODO
  }

  bool constant_value( node const& n ) const
  {
    // TODO
  }
#pragma endregion

#pragma region Create binary functions
  signal create_and( signal a, signal b )
  {
    // TODO
  }

  signal create_or( signal const& a, signal const& b )
  {
    // TODO
  }

  signal create_xor( signal const& a, signal const& b )
  {
    // TODO
  }
#pragma endregion

#pragma region Create ternary functions
  signal create_maj( signal const& a, signal const& b, signal const& c )
  {
    // TODO
  }
#pragma endregion

#pragma region Create nary functions
  signal create_nary_maj( std::vector<signal> const& fs )
  {
    // TODO
  }
#pragma endregion

#pragma region Nodes and signals
  node get_node( signal const& f ) const
  {
    // TODO
  }

  bool is_complemented( signal const& f ) const
  {
    // TODO
  }

  uint32_t node_to_index( node const& n ) const
  {
    // TODO
  }
#pragma endregion

#pragma region Node and signal iterators
  template<typename Fn>
  void foreach_pi( Fn&& fn ) const
  {
    // TODO
  }

  template<typename Fn>
  void foreach_po( Fn&& fn ) const
  {
    // TODO
  }

  template<typename Fn>
  void foreach_gate( Fn&& fn ) const
  {
    // TODO
  }

  template<typename Fn>
  void foreach_fanin( node const& n, Fn&& fn ) const
  {
    // TODO
  }
#pragma endregion

#pragma region Structural properties
  uint32_t size() const
  {
    // TODO
  }

  uint32_t num_pis() const
  {
    // TODO
  }

  uint32_t num_pos() const
  {
    // TODO
  }

  uint32_t num_gates() const
  {
    // TODO
  }

  uint32_t fanin_size( node const& n ) const
  {
    // TODO
  }

  uint32_t fanout_size( node const& n ) const
  {
    // TODO
  }
#pragma endregion

#pragma region Value simulation
  template<typename Iterator>
  iterates_over_truth_table_t<Iterator>
  compute( node const& n, Iterator begin, Iterator end ) const
  {
    (void) end;
    assert( n != 0 && !is_pi( n ) );
    typename Iterator::value_type maj_n(fanin_size(n));
    kitty::create_majority(maj_n);
    std::vector<typename Iterator::value_type> tts;
    foreach_fanin(n, [&](signal s, uint32_t i) {
      tts.push_back( is_complemented(s) ? ~(*(begin + i)) : *(begin + i));
    });
    return kitty::compose_truth_table(maj_n, tts);
  }
#pragma endregion

private:
  // TODO : storage!
};

} // namespace mockturtle
