using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Deck
    {
        [DataMember]
        public int total_cards;

        [DataMember]
        public List<string> known_cards = new List<string>();

        [DataMember]
        public List<string> joust_cards = new List<string>();

        public bool Parse(GameState game, GameState.Entity player)
        {
            bool ret = true;

            int controller = player.GetTagOrDefault(GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            this.total_cards = 0;

            foreach (var entity in game.Entities)
            {
                if (entity.Value.GetTagOrDefault(GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(GameTag.ZONE)) continue;
                if (entity.Value.GetTag(GameTag.ZONE) != (int)TAG_ZONE.DECK) continue;

                this.total_cards++;
                if (entity.Value.CardId != "") this.known_cards.Add(entity.Value.CardId);
            }

            foreach (var joust_entity_id in game.joust_information.entities)
            {
                var joust_card = game.Entities[joust_entity_id];
                if (joust_card.GetTagOrDefault(GameTag.CONTROLLER, controller - 1) != controller) continue;
                this.joust_cards.Add(joust_card.CardId);
            }

            return ret;
        }

        public override bool Equals(object obj)
        {
            Deck rhs = obj as Deck;
            if (rhs == null) return false;
            if (!this.total_cards.Equals(rhs.total_cards)) return false;
            if (!this.known_cards.SequenceEqual(rhs.known_cards)) return false;
            if (!this.joust_cards.SequenceEqual(rhs.joust_cards)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            HashHelper.Update(ref hash, this.total_cards);
            foreach (var obj in this.known_cards)
               HashHelper.Update(ref hash, obj);
            foreach (var obj in this.joust_cards)
                HashHelper.Update(ref hash, obj);
            return hash;
        }
    }
}
