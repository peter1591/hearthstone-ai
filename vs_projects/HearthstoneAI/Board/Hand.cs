using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    [DataContract]
    class Hand
    {
        [DataMember]
        public List<string> cards = new List<string>();

        public bool Parse(State.Game game, State.ReadOnlyEntity player)
        {
            bool ret = true;

            int controller = player.GetTagOrDefault(State.GameTag.CONTROLLER, -1);
            if (controller < 0) return false;

            SortedDictionary<int, int> sorted_cards = new SortedDictionary<int, int>();

            foreach (var entity in game.Entities.Items)
            {
                if (entity.Value.GetTagOrDefault(State.GameTag.CONTROLLER, controller - 1) != controller) continue;

                if (!entity.Value.HasTag(State.GameTag.ZONE)) continue;
                if (entity.Value.GetTag(State.GameTag.ZONE) != (int)State.TAG_ZONE.HAND) continue;

                var zone_pos = entity.Value.GetTagOrDefault(State.GameTag.ZONE_POSITION, -1);
                if (zone_pos < 0) ret = false;

                sorted_cards.Add(zone_pos, entity.Key);
            }

            foreach (var sorted_card in sorted_cards)
            {
                AddHandCard(game, sorted_card.Value);
            }

            return ret;
        }

        private void AddHandCard(State.Game game, int entity_id)
        {
            var entity = game.Entities.Items[entity_id];
            this.cards.Add(entity.CardId);
            // TODO:
            // fill entity id to json, instead of card id
            // another 'entities' field to json
            // each entity in 'entities' has:
            //    card id
            //    generation under blocks
        }

        public override bool Equals(object obj)
        {
            Hand rhs = obj as Hand;
            if (rhs == null) return false;
            return this.cards.SequenceEqual(rhs.cards);
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            foreach (var card in this.cards)
                HashHelper.Update(ref hash, card);
            return hash;
        }
    }
}
