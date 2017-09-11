using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    class Game
    {
        public Game()
        {
            this.Reset();
        }

        public void Reset()
        {
            this.GameEntityId = -1;
            this.PlayerEntityId = -1;
            this.OpponentEntityId = -1;
            this.entities_ = new Entities();
            this.EntityChoices = new Dictionary<int, EntityChoice>();
            this.joust_information = new JoustInformation();
            this.player_played_hand_cards.Clear();
            this.opponent_played_hand_cards.Clear();
        }

        private Entities entities_;
        public ReadOnlyEntities Entities { get { return entities_; } }
        public void AddEntity(int id, Entity data)
        {
            entities_.Add(id, data);
        }
        public void ChangeEntityCardId(int id, string card_id)
        {
            entities_.ChangeCardId(id, card_id);
        }
        public void ChangeEntityName(int id, string name)
        {
            entities_.ChangeName(id, name);
        }

        public Dictionary<int, EntityChoice> EntityChoices { get; set; }
        public int GameEntityId { get; set; }
        public int PlayerEntityId { get; set; }
        public int OpponentEntityId { get; set; }

        public JoustInformation joust_information { get; set; }

        public List<string> player_played_hand_cards = new List<string>();
        public List<string> opponent_played_hand_cards = new List<string>();

        public class StartWaitingMainActionEventArgs : EventArgs { }
        public event EventHandler<StartWaitingMainActionEventArgs> StartWaitingMainAction;

        public class EndTurnEventArgs : EventArgs
        {
            public EndTurnEventArgs() { }
            public EndTurnEventArgs(EndTurnEventArgs e) { }
        }
        public event EventHandler<EndTurnEventArgs> EndTurnEvent;

        public void CreateGameEntity(int id)
        {
            if (!entities_.Items.ContainsKey(id))
            {
                var v = new Entity(id);
                v.SetName("GameEntity");
                entities_.Add(id, v);
            }
            this.GameEntityId = id;
        }

        public bool TryGetGameEntity(out ReadOnlyEntity entity)
        {
            entity = new Entity(-1);

            if (this.GameEntityId < 0) return false;

            entity = entities_.Items[this.GameEntityId];
            return true;
        }

        public bool TryGetPlayerEntity(out ReadOnlyEntity entity)
        {
            entity = new Entity(-1);
            if (!this.entities_.Items.ContainsKey(this.PlayerEntityId)) return false;
            entity = this.entities_.Items[this.PlayerEntityId];
            return true;
        }

        public bool TryGetOpponentEntity(out ReadOnlyEntity entity)
        {
            entity = new Entity(-1);
            if (!this.entities_.Items.ContainsKey(this.OpponentEntityId)) return false;
            entity = this.entities_.Items[this.OpponentEntityId];
            return true;
        }

        public bool TryGetPlayerHeroPowerEntity(int hero_entity_id, out ReadOnlyEntity out_entity)
        {
            foreach (var entity in this.entities_.Items)
            {
                if (!entity.Value.HasTag(GameTag.CREATOR)) continue;
                if (entity.Value.GetTag(GameTag.CREATOR) != hero_entity_id) continue;

                if (!entity.Value.HasTag(GameTag.CARDTYPE)) continue;
                if (entity.Value.GetTag(GameTag.CARDTYPE) != (int)TAG_CARDTYPE.HERO_POWER) continue;

                out_entity = entity.Value;
                return true;
            }

            out_entity = new Entity(-1);
            return false;
        }

        public void ChangeTag(int entity_id, GameTag tag, int tag_value)
        {
            if (!this.entities_.Items.ContainsKey(entity_id))
            {
                this.entities_.Add(entity_id, new Entity(entity_id));
            }

            int prev_value = -1;
            bool has_prev_value = this.entities_.Items[entity_id].HasTag(tag);
            if (has_prev_value) prev_value = this.entities_.Items[entity_id].GetTag(tag);

            this.entities_.ChangeTag(entity_id, tag, tag_value);

            switch (tag)
            {
                case GameTag.MULLIGAN_STATE:
                    this.MulliganStateChanged(entity_id, GameTag.MULLIGAN_STATE, has_prev_value, prev_value, tag_value);
                    break;
            }

            // trigger end-turn action
            if (entity_id == this.GameEntityId && tag == GameTag.STEP && tag_value == (int)TAG_STEP.MAIN_END)
            {
                if (this.EndTurnEvent != null) this.EndTurnEvent(this, new EndTurnEventArgs());
            }

            if (entity_id == this.GameEntityId && tag == GameTag.STEP && tag_value == (int)TAG_STEP.MAIN_ACTION)
            {
                if (this.StartWaitingMainAction != null) StartWaitingMainAction(this, new StartWaitingMainActionEventArgs());
            }
        }

        public void ChangeTag(int entity_id, string raw_tag, string raw_value)
        {
            var tag_value = Entity.ParseTag(raw_tag, raw_value);
            this.ChangeTag(entity_id, tag_value.Item1, tag_value.Item2);
        }

        private List<Entity> tmp_entities = new List<Entity>();
        public void ChangeTag(string entity_str, string raw_tag, string raw_value)
        {
            // failed to get entity id
            // save the information to tmp_entities; insert to game state when the entity id is parsed
            var tmpEntity = this.tmp_entities.FirstOrDefault(x => x.Name == entity_str);
            if (tmpEntity == null)
            {
                tmpEntity = new Entity(this.tmp_entities.Count + 1);
                tmpEntity.SetName(entity_str);
                this.tmp_entities.Add(tmpEntity);
            }

            var tag_value = Entity.ParseTag(raw_tag, raw_value);
            tmpEntity.SetTag(tag_value.Item1, tag_value.Item2);

            if (tmpEntity.HasTag(GameTag.ENTITY_ID))
            {
                var id = tmpEntity.GetTag(GameTag.ENTITY_ID);
                entities_.ChangeName(id, tmpEntity.Name);
                foreach (var t in tmpEntity.Tags)
                    ChangeTag(id, t.Key, t.Value);
                this.tmp_entities.Remove(tmpEntity);
            }
        }

        private void MulliganStateChanged(int entity_id, GameTag tag, bool has_prev, int prev_value, int value)
        {
            // set player when mulligan

        }

        public static int ZonePositionSorter(Entity lhs, Entity rhs)
        {
            var v1 = lhs.GetTagOrDefault(GameTag.ZONE_POSITION, -1);
            var v2 = rhs.GetTagOrDefault(GameTag.ZONE_POSITION, -1);
            return v1.CompareTo(v2);
        }

        public int GetCurrentPlayerEntityId()
        {
            if (entities_.Items[PlayerEntityId].GetTagOrDefault(GameTag.CURRENT_PLAYER, 0) == 1)
            {
                return PlayerEntityId;
            }
            else if (entities_.Items[OpponentEntityId].GetTagOrDefault(GameTag.CURRENT_PLAYER, 0) == 1)
            {
                return OpponentEntityId;
            }
            else
            {
                return -1;
            }
        }
    }
}
