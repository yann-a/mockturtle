/* mockturtle: C++ logic network library
 * Copyright (C) 2018-2021  EPFL
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

 /*!
   \file simulation_cec.hpp
   \brief Simulation-based CEC

   EPFL CS-472 2021 Final Project Option 2
 */

#pragma once

#include <kitty/constructors.hpp>
#include <kitty/dynamic_truth_table.hpp>
#include <kitty/operations.hpp>

#include "../utils/node_map.hpp"
#include "miter.hpp"
#include "simulation.hpp"

namespace mockturtle {

    /* Statistics to be reported */
    struct simulation_cec_stats {
        /*! \brief Split variable (simulation size). */
        uint32_t split_var{ 0 };

        /*! \brief Number of simulation rounds. */
        uint32_t rounds{ 0 };
    };

    namespace detail {
        /*
            Class similar to default simulator, adapted to our needs
        */
        class miter_simulator {
        public:
            miter_simulator() = delete;
            miter_simulator(uint32_t num_vars, uint32_t split_var, uint32_t round) : num_vars(num_vars), split_var(split_var), round(round) {}

            kitty::dynamic_truth_table compute_constant(bool value) const {
                kitty::dynamic_truth_table tt(split_var);
                return value ? ~tt : tt;
            }

            kitty::dynamic_truth_table compute_pi(uint32_t index) const {
                kitty::dynamic_truth_table tt(split_var);

                if (index < split_var)
                    // Var index below limit : we actually simulate the variable
                    kitty::create_nth_var(tt, index);
                else
                    // Else the variable is fixed by its corresponding bit in "round"
                        // If it is 1, nothing to do : the truth table defaults to the correct one
                        // If it is 0, we negate the truth table to get the correct one
                    if (((round >> (index - split_var)) & 1) == 0)
                        tt = ~tt;

                return tt;
            }

            kitty::dynamic_truth_table compute_not(kitty::dynamic_truth_table const& value) const {
                return ~value;
            }

        private:
            uint32_t num_vars;
            uint32_t split_var;
            uint32_t round;
        };

        template<class Ntk>
        class simulation_cec_impl {
        public:
            using pattern_t = unordered_node_map<kitty::dynamic_truth_table, Ntk>;
            using node = typename Ntk::node;
            using signal = typename Ntk::signal;

        public:
            explicit simulation_cec_impl(Ntk& ntk, simulation_cec_stats& st) : _ntk(ntk), _st(st) {}

            bool run() {
                uint32_t num_pis = _ntk.num_pis();
                uint32_t num_nodes = _ntk.size();

                // Compute split var and nb of rounds, store it in stats
                _st.split_var = split_var(num_pis, num_nodes);
                _st.rounds = 1 << (num_pis - _st.split_var);

                // For every round, consider the binary value of "round" as the truth array of the fixed variables
                for (uint32_t round = 0; round < _st.rounds; round++) {
                    // Create an instance of our simulator and simulate it
                    miter_simulator sim(num_pis, _st.split_var, round);
                    const auto tts = simulate<kitty::dynamic_truth_table>(_ntk, sim);

                    // Check if it is constantly 0
                    for (auto& po : tts)
                        if (!kitty::is_const0(po))
                            return false;
                }

                return true;
            }

        private:
            /*
                Determines the split var limit

                @param num_pis   Number of inputs of the circuit
                @param num_nodes Number of nodes in the circuit
            */
            uint32_t split_var(uint32_t num_pis, uint32_t num_nodes) {
                if (num_pis <= 6) return num_pis;

                for (uint32_t m = 7; ; m++)
                    if (m == num_pis || (32 + (1 << ((m + 1) - 3))) * num_nodes > (1 << 29))
                        return m;
            }

        private:
            Ntk& _ntk;
            simulation_cec_stats& _st;
        };
    } // namespace detail

    /* Entry point for users to call */

    /*! \brief Simulation-based CEC.
     *
     * This function implements a simulation-based combinational equivalence checker.
     * The implementation creates a miter network and run several rounds of simulation
     * to verify the functional equivalence. For memory and speed reasons this approach
     * is limited up to 40 input networks. It returns an optional which is `nullopt`,
     * if the network has more than 40 inputs.
     */
    template<class Ntk>
    std::optional<bool> simulation_cec(Ntk const& ntk1, Ntk const& ntk2, simulation_cec_stats* pst = nullptr) {
        static_assert(is_network_type_v<Ntk>, "Ntk is not a network type");
        static_assert(has_num_pis_v<Ntk>, "Ntk does not implement the num_pis method");
        static_assert(has_size_v<Ntk>, "Ntk does not implement the size method");
        static_assert(has_get_node_v<Ntk>, "Ntk does not implement the get_node method");
        static_assert(has_foreach_pi_v<Ntk>, "Ntk does not implement the foreach_pi method");
        static_assert(has_foreach_po_v<Ntk>, "Ntk does not implement the foreach_po method");
        static_assert(has_foreach_node_v<Ntk>, "Ntk does not implement the foreach_node method");
        static_assert(has_is_complemented_v<Ntk>, "Ntk does not implement the is_complemented method");

        simulation_cec_stats st;

        bool result = false;

        if (ntk1.num_pis() > 40)
            return std::nullopt;

        auto ntk_miter = miter<Ntk>(ntk1, ntk2);

        if (ntk_miter.has_value()) {
            detail::simulation_cec_impl p(*ntk_miter, st);
            result = p.run();
        }

        if (pst)
            *pst = st;

        return result;
    }

} // namespace mockturtle
