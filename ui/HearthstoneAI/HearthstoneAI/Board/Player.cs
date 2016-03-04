using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Player
    {
        [DataMember]
        public Crystal crystal = new Crystal();

        [DataMember]
        public int fatigue;

        [DataMember]
        public Enchantments enchantments = new Enchantments();

        [DataMember]
        public Hero hero = new Hero();

        [DataMember]
        public Weapon weapon = new Weapon();

        [DataMember]
        public Secrets secrets = new Secrets();

        public bool Parse(GameState game, GameState.Entity entity)
        {
            bool ret = true;

            ret = this.crystal.Parse(game, entity) && ret;

            this.fatigue = entity.GetTagOrDefault(GameTag.FATIGUE, 0);

            ret = this.enchantments.Parse(game, entity) && ret;

            if (entity.HasTag(GameTag.HERO_ENTITY) == false) ret = false;
            else
            {
                int hero_entity_id = entity.GetTag(GameTag.HERO_ENTITY);
                if (game.Entities.ContainsKey(hero_entity_id) == false) ret = false;
                else {
                    ret = this.hero.Parse(game, game.Entities[hero_entity_id]) && ret;
                }
            }

            ret = this.weapon.Parse(game, entity) && ret;

            ret = this.secrets.Parse(game, entity) && ret;
            
            return ret;
        }
    }
}
