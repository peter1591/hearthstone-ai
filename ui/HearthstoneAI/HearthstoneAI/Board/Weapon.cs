using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Weapon
    {
        [DataMember]
        public bool equipped;

        [DataMember]
        public string card_id = "";

        [DataMember]
        public Enchantments enchantments = new Enchantments();

        public bool Parse(GameState game, GameState.Entity player)
        {
            bool ret = true;

            this.equipped = false;

            int controller = player.GetTagOrDefault(GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            foreach (var entity in game.Entities)
            {
                if (entity.Value.GetTagOrDefault(GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(GameTag.ZONE)) continue;
                if (entity.Value.GetTag(GameTag.ZONE) != (int)TAG_ZONE.PLAY) continue;

                if (!entity.Value.HasTag(GameTag.CARDTYPE)) continue;
                var card_type = (TAG_CARDTYPE)entity.Value.GetTag(GameTag.CARDTYPE);

                if (card_type != TAG_CARDTYPE.WEAPON) continue;

                if (this.equipped)
                {
                    // equip two weapons!
                    return false;
                }

                this.card_id = entity.Value.CardId;
                ret = this.enchantments.Parse(game, entity.Value) && ret;

                this.equipped = true;
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Weapon rhs = obj as Weapon;
            if (rhs == null) return false;
            if (!this.equipped.Equals(rhs.equipped)) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.enchantments.Equals(rhs.enchantments)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.equipped,
                this.card_id,
                this.enchantments
            }.GetHashCode();
        }
    }
}
