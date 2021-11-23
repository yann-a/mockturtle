#include <catch.hpp>

#include <mockturtle/traits.hpp>
#include <mockturtle/algorithms/aig_algebraic_rewriting.hpp>
#include <mockturtle/networks/aig.hpp>
#include <mockturtle/views/depth_view.hpp>

using namespace mockturtle;

TEST_CASE( "Simple associativity", "[aig_algebraic_rewriting]" )
{
  /* create the network */
  aig_network aig;
  const auto a = aig.create_pi();
  const auto b = aig.create_pi();
  const auto c = aig.create_pi();
  const auto d = aig.create_pi();
  const auto f1 = aig.create_and( a, b );
  const auto f2 = aig.create_and( f1, c );
  const auto f3 = aig.create_and( f2, d );
  aig.create_po( f3 );

  /* call the algorithm */
  aig_algebraic_rewriting( aig );

  /* check the result */
  depth_view depth_aig{aig};
  CHECK( depth_aig.depth() == 2 );
}
