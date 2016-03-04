using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class HeroPower
    {
        [DataMember]
        public string card_id;

        [DataMember]
        public bool used;

        public bool Parse(GameState game, GameState.Entity entity)
        {
            this.card_id = entity.CardId;
            this.used = (entity.GetTagOrDefault(GameTag.EXHAUSTED, 0) != 0);

            return true;
        }
    }
}
