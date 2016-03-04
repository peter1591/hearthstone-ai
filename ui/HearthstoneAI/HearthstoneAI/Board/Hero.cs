using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Hero
    {
        [DataMember]
        public string card_id;

        [DataMember]
        public int max_hp;

        [DataMember]
        public int damage;

        [DataMember]
        public int armor;

        [DataMember]
        public int attack;

        [DataMember]
        public int attacks_this_turn;

        [DataMember]
        public Status status = new Status();

        [DataMember]
        public HeroPower hero_power = new HeroPower();

        [DataMember]
        public Enchantments enchantments = new Enchantments();
        
        public bool Parse(GameState game, GameState.Entity entity)
        {
            bool ret = true;

            this.card_id = entity.CardId;

            this.max_hp = entity.GetTagOrDefault(GameTag.HEALTH, -1);
            this.damage = entity.GetTagOrDefault(GameTag.DAMAGE, 0);
            this.armor = entity.GetTagOrDefault(GameTag.ARMOR, 0);

            this.attack = entity.GetTagOrDefault(GameTag.ATK, 0);
            this.attacks_this_turn = entity.GetTagOrDefault(GameTag.NUM_ATTACKS_THIS_TURN, 0);

            ret = this.status.Parse(game, entity) && ret;

            GameState.Entity hero_power;
            if (game.TryGetPlayerHeroPowerEntity(entity.Id, out hero_power))
            {
                this.hero_power.Parse(game, hero_power);
            }
            else
            {
                ret = false;
            }

            ret = this.enchantments.Parse(game, entity) && ret;

            return ret;
        }
    }
}
