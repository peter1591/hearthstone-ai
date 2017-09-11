using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    class Minions : List<Minion>
    {
        public bool Parse(HearthstoneAI.Game game, HearthstoneAI.Game.Entity player)
        {
            bool ret = true;

            int controller = player.GetTagOrDefault(GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            SortedDictionary<int, Minion> sorted_minions = new SortedDictionary<int, Minion>();

            foreach (var entity in game.Entities)
            {
                if (entity.Value.GetTagOrDefault(GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(GameTag.ZONE)) continue;
                if (entity.Value.GetTag(GameTag.ZONE) != (int)TAG_ZONE.PLAY) continue;

                if (!entity.Value.HasTag(GameTag.CARDTYPE)) continue;
                var card_type = (TAG_CARDTYPE)entity.Value.GetTag(GameTag.CARDTYPE);

                if (card_type != TAG_CARDTYPE.MINION) continue;

                var zone_pos = entity.Value.GetTagOrDefault(GameTag.ZONE_POSITION, -1);
                if (zone_pos < 0) ret = false;

                Minion minion = new Minion();
                ret = minion.Parse(game, entity.Value) && ret;

                sorted_minions.Add(zone_pos, minion);
            }
            
            foreach (var sorted_minion in sorted_minions)
            {
                this.Add(sorted_minion.Value);
            }

            return true;
        }

        public override bool Equals(object obj)
        {
            Minions rhs = obj as Minions;
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
