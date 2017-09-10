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
        public bool first_player;

        [DataMember]
        public Enchantments enchantments = new Enchantments();

        [DataMember]
        public Hero hero = new Hero();

        [DataMember]
        public Weapon weapon = new Weapon();

        [DataMember]
        public Secrets secrets = new Secrets();

        [DataMember]
        public Minions minions = new Minions();

        [DataMember]
        public Hand hand = new Hand();

        [DataMember]
        public Deck deck = new Deck();

        public bool Parse(GameState game, GameState.Entity entity)
        {
            bool ret = true;

            ret = this.crystal.Parse(game, entity) && ret;

            this.fatigue = entity.GetTagOrDefault(GameTag.FATIGUE, 0);

            this.first_player = (entity.GetTagOrDefault(GameTag.FIRST_PLAYER, 0) != 0);

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

            ret = this.minions.Parse(game, entity) && ret;

            ret = this.hand.Parse(game, entity) && ret;

            ret = this.deck.Parse(game, entity) && ret;
            
            return ret;
        }

        public override bool Equals(object obj)
        {
            Player rhs = obj as Player;
            if (rhs == null) return false;
            if (!this.crystal.Equals(rhs.crystal)) return false;
            if (!this.fatigue.Equals(rhs.fatigue)) return false;
            if (!this.first_player.Equals(rhs.first_player)) return false;
            if (!this.enchantments.Equals(rhs.enchantments)) return false;
            if (!this.hero.Equals(rhs.hero)) return false;
            if (!this.weapon.Equals(rhs.weapon)) return false;
            if (!this.secrets.Equals(rhs.secrets)) return false;
            if (!this.minions.Equals(rhs.minions)) return false;
            if (!this.hand.Equals(rhs.hand)) return false;
            if (!this.deck.Equals(rhs.deck)) return false;

            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.crystal,
                this.fatigue,
                this.first_player,
                this.enchantments,
                this.hero,
                this.weapon,
                this.secrets,
                this.minions,
                this.hand,
                this.deck
            }.GetHashCode();
        }
    }
}
