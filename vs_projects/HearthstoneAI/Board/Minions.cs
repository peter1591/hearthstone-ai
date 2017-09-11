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
        public bool Parse(State.Game game, State.Game.Entity player)
        {
            bool ret = true;

            int controller = player.GetTagOrDefault(State.GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            SortedDictionary<int, Minion> sorted_minions = new SortedDictionary<int, Minion>();

            foreach (var entity in game.Entities)
            {
                if (entity.Value.GetTagOrDefault(State.GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(State.GameTag.ZONE)) continue;
                if (entity.Value.GetTag(State.GameTag.ZONE) != (int)State.TAG_ZONE.PLAY) continue;

                if (!entity.Value.HasTag(State.GameTag.CARDTYPE)) continue;
                var card_type = (State.TAG_CARDTYPE)entity.Value.GetTag(State.GameTag.CARDTYPE);

                if (card_type != State.TAG_CARDTYPE.MINION) continue;

                var zone_pos = entity.Value.GetTagOrDefault(State.GameTag.ZONE_POSITION, -1);
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
