using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    class Enchantments : List<Enchantment>
    {
        public bool Parse(State.Game game, State.ReadOnlyEntity target)
        {
            bool ret = true;

            foreach (var entity in game.Entities.Items)
            {
                if (!entity.Value.HasTag(State.GameTag.CARDTYPE)) continue;
                var card_type = (State.TAG_CARDTYPE)entity.Value.GetTag(State.GameTag.CARDTYPE);

                if (card_type != State.TAG_CARDTYPE.ENCHANTMENT) continue;

                if (entity.Value.GetTagOrDefault(State.GameTag.ATTACHED, target.Id - 1) != target.Id) continue;

                var enchant = new Enchantment();
                ret = enchant.Parse(game, entity.Value) && ret;
                this.Add(enchant);
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Enchantments rhs = obj as Enchantments;
            if (rhs == null) return false;
            return this.SequenceEqual(rhs);
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            foreach (var obj in this)
                HashHelper.Update(ref hash, obj);
            return hash;
        }
    }
}
