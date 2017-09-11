using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    class Secrets : List<Secret>
    {
        public bool Parse(State.Game game, State.Entity player)
        {
            bool ret = true;

            int controller = player.GetTagOrDefault(State.GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            foreach (var entity in game.Entities)
            {
                if (entity.Value.GetTagOrDefault(State.GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(State.GameTag.ZONE)) continue;
                if (entity.Value.GetTag(State.GameTag.ZONE) != (int)State.TAG_ZONE.SECRET) continue;

                Secret secret = new Secret();
                ret = secret.Parse(game, entity.Value) && ret;
                this.Add(secret);
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Secrets rhs = obj as Secrets;
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
