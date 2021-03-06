/**
 * \file dpo_map.hpp
 *
 * \copyright 2018 John Harwell, All rights reserved.
 *
 * This file is part of FORDYCA.
 *
 * FORDYCA is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * FORDYCA is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * FORDYCA.  If not, see <http://www.gnu.org/licenses/
 */

#ifndef INCLUDE_FORDYCA_DS_DPO_MAP_HPP_
#define INCLUDE_FORDYCA_DS_DPO_MAP_HPP_

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <boost/range/adaptor/map.hpp>
#include <map>
#include <utility>

#include "fordyca/repr/dpo_entity.hpp"

/*******************************************************************************
 * Namespaces/Decls
 ******************************************************************************/
NS_START(fordyca, ds);

/*******************************************************************************
 * Class Definitions
 ******************************************************************************/
/**
 * \class dpo_map
 * \ingroup ds
 *
 * \brief The Decaying Pheromone Object (DPO) map stores objects in the arena
 * SEPARATELY from the \ref arena_map where they actually live (clone not
 * reference), which decouples/simplifies a lot of the tricky handshaking logic
 * when robots interact with the arena.
 */
template <typename key_type, typename obj_type>
class dpo_map {
 public:
  using value_type = typename repr::dpo_entity<obj_type>;
  using map_type = std::map<key_type, value_type>;

  template <typename T, typename Adaptor>
  using iterator_type =
      decltype(boost::make_iterator_range(std::declval<T>().begin(),
                                          std::declval<T>().end()) |
               std::declval<Adaptor>());

  dpo_map(void) : mc_obj_ref(m_obj) {}

  /**
   * \brief Update the densities of all objects in the map. Should be called
   * when one unit of time has passed (e.g. every timestep).
   */
  void decay_all(void) {
    for (std::pair<const key_type, value_type>& o : m_obj) {
      o.second.density().update();
    } /* for(&o..) */
  }

  /**
   * \brief Returns a pointer to the object that matches the specified key, or
   * nullptr if the key is not found in themap.
   */
  const value_type* find(const key_type& key) const RCPPSW_PURE {
    auto it = m_obj.find(key);
    return (it == m_obj.end()) ? nullptr : &(it->second);
  }
  value_type* find(const key_type& key) RCPPSW_PURE {
    auto it = m_obj.find(key);
    return (it == m_obj.end()) ? nullptr : &(it->second);
  }

  /**
   * \brief Returns \c TRUE iff the key is contained in the map, and \c FALSE
   * otherwise.
   */
  RCPPSW_PURE bool contains(const key_type& key) const {
    return nullptr != find(key);
  }

  /**
   * \brief Add the specified object from the map of known objects of that
   * type. If it is already in the map of known objects of that type, the old
   * version is replaced.
   */
  void obj_add(std::pair<key_type, value_type>&& obj) {
    m_obj.erase(obj.first);
    m_obj.insert(std::move(obj));
  }

  /**
   * \brief Return an iterator for examining, but not modifying the values of
   * the map.
   */
  auto const_values_range(void) const
      -> decltype(std::declval<map_type>() | boost::adaptors::map_values) {
    return m_obj | boost::adaptors::map_values;
  }

  /**
   * \brief Return an iterator for examining, but not modifying, the keys of
   * the map.
   */
  iterator_type<const map_type, decltype(boost::adaptors::map_keys)>
  keys_range(void) const {
    return boost::make_iterator_range(m_obj.begin(), m_obj.end()) |
           boost::adaptors::map_keys;
  }

  /**
   * \brief Remove the specified object from the map of known objects of that
   * type (if it exists). If the argument is not in the map of known objects of
   * that type, no action is performed.
   */
  void obj_remove(const key_type& key) { m_obj.erase(key); }

 private:
  /* clang-format off */
  /**
   * \brief Needed for compiler to correctly deduce wrapped function return
   * types for const qualified contexts (must be BEFORE the wrapping macros in
   * the file).
   */
  const map_type& mc_obj_ref;
  map_type        m_obj{};
  /* clang-format on */

 public:
  RCPPSW_WRAP_DECLDEF(size, mc_obj_ref, const)
  RCPPSW_WRAP_DECLDEF(empty, mc_obj_ref, const)
  RCPPSW_WRAP_DECLDEF(clear, m_obj)

  /**
   * \brief Iterate over mutable values of the map.
   *
   * \todo This has to be AFTER the member variable is declared, because I can't
   * figure out how to get std:declval<map_type>() to be non-const (I think) and
   * have the non-const map_values iterator as the chosen function overload.
   */
  auto values_range(void) -> decltype(m_obj | boost::adaptors::map_values) {
    return m_obj | boost::adaptors::map_values;
  }
};

NS_END(ds, fordyca);

#endif /* INCLUDE_FORDYCA_DS_DPO_MAP_HPP_ */
