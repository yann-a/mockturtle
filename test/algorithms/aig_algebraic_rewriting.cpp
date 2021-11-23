#include <catch.hpp>

#include <mockturtle/algorithms/aig_algebraic_rewriting_sol.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/depth_view.hpp>
#include <mockturtle/algorithms/simulation.hpp>
#include <kitty/static_truth_table.hpp>

using namespace mockturtle;

TEST_CASE( "Simple associativity", "[aig_algebraic_rewriting]" )
{
  /* create the network */
  aig_network aig;
  static const uint32_t num_pis{4};
  std::vector<typename aig_network::signal> pis;
  for ( uint32_t i = 0; i < num_pis; ++i )
    pis.emplace_back( aig.create_pi() );

  const auto f1 = aig.create_and( pis[0], pis[1] );
  const auto f2 = aig.create_and( f1, pis[2] );
  const auto f3 = aig.create_and( f2, pis[3] );
  aig.create_po( f3 );

  /* simulate to get the output truth table(s) */
  auto tts = simulate<kitty::static_truth_table<num_pis>>( aig );

  /* call the algorithm */
  aig_algebraic_rewriting( aig );

  /* check the resulting depth */
  depth_view depth_aig{aig};
  CHECK( depth_aig.depth() == 2 );

  /* check that the output functions remain the same */
  CHECK( tts == simulate<kitty::static_truth_table<num_pis>>( aig ) );
}
