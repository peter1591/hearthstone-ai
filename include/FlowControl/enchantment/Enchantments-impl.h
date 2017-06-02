#pragma once

#include "FlowControl/enchantment/Enchantments.h"

#include "state/State.h"
#include "FlowControl/enchantment/detail/UpdateDecider-impl.h"

namespace FlowControl {
namespace enchantment {
inline bool Enchantments::NormalEnchantment::Apply(ApplyFunctorContext const& context) const {
  if (valid_until_turn > 0) {
    if (context.state_.GetTurn() > valid_until_turn) {
      return false;
    }
  }
  apply_functor(context);
  return true;
}

  template <typename EnchantmentType>
inline void Enchantments::PushBackNormalEnchantment(state::State const& state, EnchantmentType&& item)
{
  update_decider_.AddItem();
  int valid_until_turn = -1;
  if (item.valid_this_turn) valid_until_turn = state.GetTurn();
  update_decider_.AddValidUntilTurn(valid_until_turn);

  if (item.force_update_every_time) update_decider_.AddForceUpdateItem();

  assert(item.apply_functor);
  enchantments_.PushBack(NormalEnchantment(item.apply_functor, valid_until_turn, item.force_update_every_time));
}
}
}
