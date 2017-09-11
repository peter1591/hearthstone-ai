using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Crystal
    {
        [DataMember]
        public int total;

        [DataMember]
        public int used;

        [DataMember]
        public int this_turn;

        [DataMember]
        public int overload;

        [DataMember]
        public int overload_next_turn;

        public bool Parse(HearthstoneAI.Game game, HearthstoneAI.Game.Entity entity)
        {
            this.total = entity.GetTagOrDefault(GameTag.RESOURCES, 0);
            this.this_turn = entity.GetTagOrDefault(GameTag.TEMP_RESOURCES, 0);
            this.used = entity.GetTagOrDefault(GameTag.RESOURCES_USED, 0);

            this.overload = entity.GetTagOrDefault(GameTag.OVERLOAD_LOCKED, 0);
            this.overload_next_turn = entity.GetTagOrDefault(GameTag.OVERLOAD_OWED, 0);

            return true;
        }

        public override bool Equals(object obj)
        {
            Crystal rhs = obj as Crystal;
            if (rhs == null) return false;
            if (!this.total.Equals(rhs.total)) return false;
            if (!this.used.Equals(rhs.used)) return false;
            if (!this.this_turn.Equals(rhs.this_turn)) return false;
            if (!this.overload.Equals(rhs.overload)) return false;
            if (!this.overload_next_turn.Equals(rhs.overload_next_turn)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.total,
                this.used,
                this.this_turn,
                this.overload,
                this.overload_next_turn
            }.GetHashCode();
        }
    }
}
