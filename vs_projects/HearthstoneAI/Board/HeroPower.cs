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
        public int entity_id;

        [DataMember]
        public string card_id;

        [DataMember]
        public bool used;

        public bool Parse(State.Game game, State.ReadOnlyEntity entity)
        {
            this.entity_id = entity.Id;
            this.card_id = entity.CardId;
            this.used = (entity.GetTagOrDefault(State.GameTag.EXHAUSTED, 0) != 0);

            return true;
        }

        public override bool Equals(object obj)
        {
            HeroPower rhs = obj as HeroPower;
            if (rhs == null) return false;
            if (!this.entity_id.Equals(rhs.entity_id)) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.used.Equals(rhs.used)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.entity_id,
                this.card_id,
                this.used
            }.GetHashCode();
        }
    }
}
