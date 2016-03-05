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
        public Status status = new Status();

        [DataMember]
        public Enchantments enchantments = new Enchantments();

        public bool Parse(GameState game, GameState.Entity entity)
        {
            this.entity_id = entity.Id;
            this.card_id = entity.CardId;

            this.max_hp = entity.GetTagOrDefault(GameTag.HEALTH, -1);
            this.damage = entity.GetTagOrDefault(GameTag.DAMAGE, 0);
            this.attack = entity.GetTagOrDefault(GameTag.ATK, 0);
            this.attacks_this_turn = entity.GetTagOrDefault(GameTag.NUM_ATTACKS_THIS_TURN, 0);
            this.exhausted = (entity.GetTagOrDefault(GameTag.EXHAUSTED, 0) != 0);
            this.silenced = (entity.GetTagOrDefault(GameTag.SILENCED, 0) != 0);

            this.status.Parse(game, entity);
            this.enchantments.Parse(game, entity);

            return true;
        }
    }
}
