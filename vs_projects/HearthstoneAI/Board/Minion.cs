using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Minion
    {
        [DataMember]
        public int entity_id;

        [DataMember]
        public string card_id;

        [DataMember]
        public int max_hp;

        [DataMember]
        public int damage;

        [DataMember]
        public int attack;

        [DataMember]
        public int attacks_this_turn;

        [DataMember]
        public bool exhausted;

        [DataMember]
        public bool silenced;

        [DataMember]
        public int spellpower;

        [DataMember]
        public Status status = new Status();

        [DataMember]
        public bool summoned_this_turn;

        [DataMember]
        public Enchantments enchantments = new Enchantments();

        public bool Parse(State.Game game, State.ReadOnlyEntity entity)
        {
            this.entity_id = entity.Id;
            this.card_id = entity.CardId;

            this.max_hp = entity.GetTagOrDefault(State.GameTag.HEALTH, -1);
            this.damage = entity.GetTagOrDefault(State.GameTag.DAMAGE, 0);
            this.attack = entity.GetTagOrDefault(State.GameTag.ATK, 0);
            this.attacks_this_turn = entity.GetTagOrDefault(State.GameTag.NUM_ATTACKS_THIS_TURN, 0);
            this.exhausted = (entity.GetTagOrDefault(State.GameTag.EXHAUSTED, 0) != 0);
            this.silenced = (entity.GetTagOrDefault(State.GameTag.SILENCED, 0) != 0);
            this.spellpower = entity.GetTagOrDefault(State.GameTag.SPELLPOWER, 0);
            this.summoned_this_turn = entity.GetTagOrDefault(State.GameTag.JUST_PLAYED, 0) != 0;

            this.status.Parse(game, entity);
            this.enchantments.Parse(game, entity);

            return true;
        }

        public override bool Equals(object obj)
        {
            Minion rhs = obj as Minion;
            if (rhs == null) return false;
            if (!this.entity_id.Equals(rhs.entity_id)) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.max_hp.Equals(rhs.max_hp)) return false;
            if (!this.damage.Equals(rhs.damage)) return false;
            if (!this.attack.Equals(rhs.attack)) return false;
            if (!this.attacks_this_turn.Equals(rhs.attacks_this_turn)) return false;
            if (!this.exhausted.Equals(rhs.exhausted)) return false;
            if (!this.silenced.Equals(rhs.silenced)) return false;
            if (!this.spellpower.Equals(rhs.spellpower)) return false;
            if (!this.status.Equals(rhs.status)) return false;
            if (!this.summoned_this_turn.Equals(rhs.summoned_this_turn)) return false;
            if (!this.enchantments.Equals(rhs.enchantments)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.entity_id,
                this.card_id,
                this.max_hp,
                this.damage,
                this.attack,
                this.attacks_this_turn,
                this.exhausted,
                this.silenced,
                this.spellpower,
                this.status,
                this.summoned_this_turn,
                this.enchantments
            }.GetHashCode();
        }
    }
}
