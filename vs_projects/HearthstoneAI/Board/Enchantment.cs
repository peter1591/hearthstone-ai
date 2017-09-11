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

        public bool Parse(HearthstoneAI.Game game, HearthstoneAI.Game.Entity enchant)
        {
            this.card_id = enchant.CardId;
            this.creator_entity_id = enchant.GetTagOrDefault(GameTag.CREATOR, -1);

            return true;
        }

        public override bool Equals(object obj)
        {
            Enchantment rhs = obj as Enchantment;
            if (rhs == null) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.creator_entity_id.Equals(rhs.creator_entity_id)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.card_id,
                this.creator_entity_id
            }.GetHashCode();
        }
    }
}
