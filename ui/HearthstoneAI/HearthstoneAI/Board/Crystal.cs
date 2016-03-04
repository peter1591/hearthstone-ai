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

        public bool Parse(GameState game, GameState.Entity entity)
        {
            this.total = entity.GetTagOrDefault(GameTag.RESOURCES, 0);
            this.this_turn = entity.GetTagOrDefault(GameTag.TEMP_RESOURCES, 0);
            this.used = entity.GetTagOrDefault(GameTag.RESOURCES_USED, 0);

            this.overload = entity.GetTagOrDefault(GameTag.OVERLOAD_LOCKED, 0);
            this.overload_next_turn = entity.GetTagOrDefault(GameTag.OVERLOAD_OWED, 0);

            return true;
        }
    }
}
