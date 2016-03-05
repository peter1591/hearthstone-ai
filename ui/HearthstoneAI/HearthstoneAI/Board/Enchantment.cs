using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Enchantment
    {
        [DataMember]
        public string card_id;

        [DataMember]
        public int creator_entity_id;

        public bool Parse(GameState game, GameState.Entity enchant)
        {
            this.card_id = enchant.CardId;
            this.creator_entity_id = enchant.GetTagOrDefault(GameTag.CREATOR, -1);

            return true;
        }
    }
}
