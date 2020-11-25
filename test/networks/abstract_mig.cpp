#include <catch.hpp>

#include <kitty/dynamic_truth_table.hpp>
#include <kitty/print.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <mockturtle/generators/majority_n.hpp>
#include <mockturtle/networks/abstract_mig.hpp>
#include <mockturtle/networks/mig.hpp>
#include <mockturtle/traits.hpp>
#include <random>

namespace mockturtle {

template<typename Ntk>
class my_random_logic_generator
{
public:
  using node = typename Ntk::node;
  using signal = typename Ntk::signal;

public:
  struct rule
  {
    std::function<signal(Ntk&, std::vector<signal> const&)> func;
    uint32_t num_args;
  };

  using rules_t = std::vector<rule>;

public:
  explicit my_random_logic_generator( rules_t const &gens )
    : _gens( gens )
  {
  }

  Ntk generate( uint32_t num_inputs, uint32_t num_gates, uint64_t seed = 0xcafeaffe ) const
  {
    assert( num_inputs > 0 );
    assert( num_gates > 0 );

    std::vector<signal> fs;
    Ntk ntk;

    /* generate constant */
    fs.emplace_back( ntk.get_constant( 0 ) );

    /* generate pis */
    for ( auto i = 0u; i < num_inputs; ++i )
    {
      fs.emplace_back( ntk.create_pi() );
    }

    /* generate gates */
    std::mt19937 rng( static_cast<unsigned int>( seed ) );
    std::uniform_int_distribution<int> rule_dist( 0, static_cast<int>( _gens.size() - 1u ) );

    auto gate_counter = 0u;
    while ( gate_counter < num_gates )
    {
      auto const r = _gens.at( rule_dist( rng ) );

      std::uniform_int_distribution<int> dist( 0, static_cast<int>( fs.size() - 1 ) );
      std::vector<signal> args;
      for ( auto i = 0u; i < r.num_args; ++i )
      {
        auto const a_compl = dist( rng ) & 1;
        auto const a = fs.at( dist( rng ) );
        args.emplace_back( a_compl ? !a : a );
      }

      auto const g = r.func( ntk, args );
      ntk.create_po( g );
      fs.emplace_back( g );
      ++gate_counter;
    }
    return ntk;
  }

  rules_t const _gens;
};

/*! \brief Generates a random MIG network MAJ-, AND-, and OR-gates */
inline my_random_logic_generator<mig_network> mig_random_maj_n_generator()
{
  using gen_t = my_random_logic_generator<mig_network>;

  gen_t::rules_t rules;
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 3u );
      return mig.create_maj( vs[0], vs[1], vs[2] );
    }, 3u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 5u );
      return majority_n_bdd( mig, std::array<mig_network::signal, 5>({vs[0], vs[1], vs[2], vs[3], vs[4]}) );
    }, 5u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 7u );
      return majority_n_bdd( mig, std::array<mig_network::signal, 7>({vs[0], vs[1], vs[2], vs[3], vs[4], vs[5], vs[6]}) );
    }, 7u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 9u );
      return majority_n_bdd( mig, std::array<mig_network::signal, 9>({vs[0], vs[1], vs[2], vs[3], vs[4], vs[5], vs[6], vs[7], vs[8]}) );
    }, 9u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_and( vs[0], vs[1] );
    }, 2u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_or( vs[0], vs[1] );
    }, 2u} );
  rules.emplace_back( gen_t::rule{[]( mig_network& mig, std::vector<mig_network::signal> const& vs ) -> mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_xor( vs[0], vs[1] );
    }, 2u} );

  return my_random_logic_generator<mig_network>( rules );
}

/*! \brief Generates a random MIG network MAJ-, AND-, and OR-gates */
inline my_random_logic_generator<abstract_mig_network> abstract_mig_random_maj_n_generator()
{
  using gen_t = my_random_logic_generator<abstract_mig_network>;

  gen_t::rules_t rules;
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 3u );
      return mig.create_maj( vs[0], vs[1], vs[2] );
    }, 3u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 5u );
      return mig.create_nary_maj( vs );
    }, 5u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 7u );
      return mig.create_nary_maj( vs );
    }, 7u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 9u );
      return mig.create_nary_maj( vs );
    }, 9u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_and( vs[0], vs[1] );
    }, 2u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_or( vs[0], vs[1] );
    }, 2u} );
  rules.emplace_back( gen_t::rule{[]( abstract_mig_network& mig, std::vector<abstract_mig_network::signal> const& vs ) -> abstract_mig_network::signal
    {
      assert( vs.size() == 2u );
      return mig.create_xor( vs[0], vs[1] );
    }, 2u} );

  return my_random_logic_generator<abstract_mig_network>( rules );
}

}

using namespace mockturtle;

TEST_CASE( "Create a majority-n in abstract MIG", "[abstract_mig]" )
{
  for (uint32_t i = 3; i < 16; i += 2) {
    abstract_mig_network mig;
    std::vector<abstract_mig_network::signal> pis;
    for (uint32_t j = 0; j < i; ++j) {
      pis.push_back( mig.create_pi() );
    }
    const auto f = mig.create_nary_maj( pis );
    mig.create_po( f );

    CHECK( mig.num_pis() == i );
    CHECK( mig.num_pos() == 1u );
    CHECK( mig.num_gates() == 1u );

    default_simulator<kitty::dynamic_truth_table> sim( mig.num_pis() );
    const auto result = simulate<kitty::dynamic_truth_table>( mig, sim );

    kitty::dynamic_truth_table maj_tt(mig.num_pis());
    kitty::create_majority(maj_tt);
    CHECK( result[0] == maj_tt );
  }
}

TEST_CASE( "Create random MIGs", "[abstract_mig]" )
{
  auto const mig_gen = mig_random_maj_n_generator();
  auto const abstract_mig_gen = abstract_mig_random_maj_n_generator();

  for (uint32_t i = 4; i < 14; ++i) {
    auto const mig = mig_gen.generate( i, i * 10u );
    auto const abstract_mig = abstract_mig_gen.generate( i, i * 10u );

    default_simulator<kitty::dynamic_truth_table> sim0( mig.num_pis() );
    const auto r0 = simulate<kitty::dynamic_truth_table>( mig, sim0 );

    default_simulator<kitty::dynamic_truth_table> sim1( abstract_mig.num_pis() );
    const auto r1 = simulate<kitty::dynamic_truth_table>( abstract_mig, sim1 );

    CHECK(r0 == r1);
  }
}
