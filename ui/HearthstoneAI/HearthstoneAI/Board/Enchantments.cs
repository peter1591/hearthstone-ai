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
        public bool Parse(GameState game, GameState.Entity target)
        {
            bool ret = true;

            foreach (var entity in game.Entities)
            {
                if (!entity.Value.HasTag(GameTag.CARDTYPE)) continue;
                var card_type = (TAG_CARDTYPE)entity.Value.GetTag(GameTag.CARDTYPE);

                if (card_type != TAG_CARDTYPE.ENCHANTMENT) continue;

                if (entity.Value.GetTagOrDefault(GameTag.ATTACHED, target.Id - 1) != target.Id) continue;

                var enchant = new Enchantment();
                ret = enchant.Parse(game, entity.Value) && ret;
                this.Add(enchant);
            }

            return ret;
        }

    }
}
