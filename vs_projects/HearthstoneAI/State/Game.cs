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
        public void CreateEntity(int id)
        {
            entities_.Add(id, new Entity(id, blocks_));
        }
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
                var v = new Entity(id, blocks_);
                v.SetName("GameEntity");
                entities_.Add(id, v);
            }
            this.GameEntityId = id;
        }

        public bool TryGetGameEntity(out ReadOnlyEntity entity)
        {
            if (this.GameEntityId < 0)
            {
                entity = null;
                return false;
            }

            entity = entities_.Items[this.GameEntityId];
            return true;
        }

        public bool TryGetPlayerEntity(out ReadOnlyEntity entity)
        {
            if (!this.entities_.Items.ContainsKey(this.PlayerEntityId))
            {
                entity = null;
                return false;
            }
            entity = this.entities_.Items[this.PlayerEntityId];
            return true;
        }

        public bool TryGetOpponentEntity(out ReadOnlyEntity entity)
        {
            if (!this.entities_.Items.ContainsKey(this.OpponentEntityId))
            {
                entity = null;
                return false;
            }
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

            out_entity = null;
            return false;
        }

        public void ChangeTag(int entity_id, GameTag tag, int tag_value)
        {
            if (!this.entities_.Items.ContainsKey(entity_id))
            {
                this.entities_.Add(entity_id, new Entity(entity_id, blocks_));
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
                tmpEntity = new Entity(this.tmp_entities.Count + 1, blocks_);
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

        public void NotifyEntityTagChanged(int id, State.ReadOnlyEntity prev, LogWatcher.Logger logger)
        {
            State.ReadOnlyEntity current = entities_.Items[id];
            int current_controller = current.GetTagOrDefault(GameTag.CONTROLLER, -1);
            TAG_ZONE current_zone = (TAG_ZONE)current.GetTagOrDefault(GameTag.ZONE, (int)TAG_ZONE.INVALID);
            if (current_controller < 0) return;
            if (current_zone == TAG_ZONE.INVALID) return;

            if (prev != null)
            {
                if (prev.Id != id)
                {
                    logger.Info("entity id should not be changed.");
                    return;
                }
                int prev_controller = prev.GetTagOrDefault(GameTag.CONTROLLER, -1);
                TAG_ZONE prev_zone = (TAG_ZONE)prev.GetTagOrDefault(GameTag.ZONE, (int)TAG_ZONE.INVALID);

                if (prev_controller >= 0 && prev_zone != TAG_ZONE.INVALID)
                {
                    if (prev_controller == current_controller && prev_zone == current_zone) return;
                    EntityZoneChanged(id, prev_zone, prev_controller, current_zone, current_controller, logger);
                }
            }

            EntityAdded(id, current_zone, current_controller, logger);
        }

        private void EntityAdded(int id, TAG_ZONE zone, int controller, LogWatcher.Logger logger)
        {
            if (zone == TAG_ZONE.DECK)
            {
                logger.Info(String.Format(
                    "New Entity {0} to controller {1}'s deck.",
                    id, controller.ToString()));
            }
        }

        private void EntityZoneChanged(
            int id,
            TAG_ZONE prev_zone, int prev_controller,
            TAG_ZONE current_zone, int current_controller,
            LogWatcher.Logger logger)
        {
            if (prev_zone == TAG_ZONE.DECK)
            {
                logger.Info(string.Format(
                    "Entity {0} moved from controller {1}'s deck",
                    id.ToString(), prev_controller.ToString()));
            }
        }

        public List<State.BlockInfo> blocks_ = new List<BlockInfo>();
        public void NotifyBlockStarted(int entity_id, string block_type)
        {
            blocks_.Add(new State.BlockInfo()
            {
                entity_id = entity_id,
                block_type = block_type
            });
        }
        public void NotifyBlockEnded(int entity_id, string block_type, LogWatcher.Logger logger)
        {
            if (blocks_[blocks_.Count - 1].block_type != block_type)
            {
                logger.Info("block type not matched in BlockEnd");
            }
            else if (blocks_[blocks_.Count - 1].entity_id != entity_id)
            {
                logger.Info("entity id not matched in BlockEnd");
            }
            else
            {
                blocks_.RemoveAt(blocks_.Count - 1);
            }
        }
    }
}
