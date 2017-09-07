using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI
{
    class GameState
    {
        public class Entity
        {
            public Entity(int id)
            {
                Id = id;
                Tags = new Dictionary<GameTag, int>();
                Name = "";
                CardId = "";
            }

            public int Id { get; }
            public Dictionary<GameTag, int> Tags { get; }
            public string Name { get; set; }
            public string CardId { get; set; }

            public bool HasTag(GameTag tag)
            {
                return Tags.ContainsKey(tag);
            }

            public int GetTagOrDefault(GameTag tag, int default_val)
            {
                if (!HasTag(tag)) return default_val;
                return GetTag(tag);
            }

            public int GetTag(GameTag tag)
            {
                int value;
                if (Tags.TryGetValue(tag, out value) == false)
                {
                    throw new Exception("tag not found");
                }
                return value;
            }

            public void SetTag(GameTag tag, int value)
            {
                if (!Tags.ContainsKey(tag))
                    Tags.Add(tag, value);
                else
                    Tags[tag] = value;
            }

            public static Tuple<GameTag, int> ParseTag(string raw_tag, string raw_value)
            {
                GameTag tag;
                if (!Enum.TryParse(raw_tag, out tag))
                {
                    int raw_tag_int;
                    if (!int.TryParse(raw_tag, out raw_tag_int))
                    {
                        return new Tuple<GameTag, int>(GameTag.INVALID, -1);
                    }

                    if (Enum.IsDefined(typeof(GameTag), raw_tag_int))
                    {
                        tag = (GameTag)raw_tag_int;
                    }
                }

                var value = ParseTagValue(tag, raw_value);

                return new Tuple<GameTag, int>(tag, value);
            }

            private static int ParseTagValue(GameTag tag, string raw_value)
            {
                switch (tag)
                {
                    case GameTag.STEP:
                        TAG_STEP step;
                        if (Enum.TryParse(raw_value, out step)) return (int)step;
                        break;

                    case GameTag.ZONE:
                        TAG_ZONE zone;
                        if (Enum.TryParse(raw_value, out zone)) return (int)zone;
                        break;

                    case GameTag.MULLIGAN_STATE:
                        {
                            TAG_MULLIGAN state;
                            if (Enum.TryParse(raw_value, out state)) return (int)state;
                        }
                        break;
                    case GameTag.PLAYSTATE:
                        {
                            TAG_PLAYSTATE state;
                            if (Enum.TryParse(raw_value, out state)) return (int)state;
                        }
                        break;
                    case GameTag.CARDTYPE:
                        TAG_CARDTYPE type;
                        if (Enum.TryParse(raw_value, out type)) return (int)type;
                        break;

                    case GameTag.CLASS:
                        TAG_CLASS @class;
                        if (Enum.TryParse(raw_value, out @class)) return (int)@class;
                        break;

                    case GameTag.STATE:
                        {
                            TAG_STATE state;
                            if (Enum.TryParse(raw_value, out state)) return (int)state;
                        }
                        break;

                    default:
                        int value;
                        if (int.TryParse(raw_value, out value)) return value;
                        break;
                }

                return -1;
            }
        }

        public class EntityChoice
        {
            public EntityChoice()
            {
                choices = new Dictionary<int, int>();
                choices_has_sent = false;
                sent_choices = new List<int>();
            }

            public int id;
            public string choice_type;
            public int player_entity_id;
            public string source;
            public Dictionary<int, int> choices;

            public bool choices_has_sent;
            public List<int> sent_choices;
        }

        public class JoustInformation
        {
            public JoustInformation() { this.entities = new List<int>(); }

            public int AddEntityId(int entity_id)
            {
                if (this.entities.Contains(entity_id)) return 0;
                this.entities.Add(entity_id);
                return 1;
            }

            public readonly List<int> entities;
        }

        public GameState()
        {
            this.Reset();
        }

        public void Reset()
        {
            this.GameEntityId = -1;
            this.PlayerEntityId = -1;
            this.OpponentEntityId = -1;
            this.Entities = new Dictionary<int, Entity>();
            this.EntityChoices = new Dictionary<int, EntityChoice>();
            this.joust_information = new GameState.JoustInformation();
            this.player_played_hand_cards.Clear();
            this.opponent_played_hand_cards.Clear();
        }

        public Dictionary<int, Entity> Entities { get; set; }
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
            if (!Entities.ContainsKey(id))
            {
                Entities.Add(id, new GameState.Entity(id) { Name = "GameEntity" });
            }
            this.GameEntityId = id;
        }

        public bool TryGetGameEntity(out Entity entity)
        {
            entity = new Entity(-1);

            if (this.GameEntityId < 0) return false;

            entity = Entities[this.GameEntityId];
            return true;
        }

        public bool TryGetPlayerEntity(out Entity entity)
        {
            entity = new Entity(-1);
            if (!this.Entities.ContainsKey(this.PlayerEntityId)) return false;
            entity = this.Entities[this.PlayerEntityId];
            return true;
        }

        public bool TryGetOpponentEntity(out Entity entity)
        {
            entity = new Entity(-1);
            if (!this.Entities.ContainsKey(this.OpponentEntityId)) return false;
            entity = this.Entities[this.OpponentEntityId];
            return true;
        }

        public bool TryGetPlayerHeroPowerEntity(int hero_entity_id, out Entity out_entity)
        {
            foreach (var entity in this.Entities)
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
            if (!this.Entities.ContainsKey(entity_id))
            {
                this.Entities.Add(entity_id, new Entity(entity_id));
            }

            int prev_value = -1;
            bool has_prev_value = this.Entities[entity_id].HasTag(tag);
            if (has_prev_value) prev_value = this.Entities[entity_id].GetTag(tag);

            this.Entities[entity_id].SetTag(tag, tag_value);

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
            if (Entities[PlayerEntityId].GetTagOrDefault(GameTag.CURRENT_PLAYER, 0) == 1)
            {
                return PlayerEntityId;
            }
            else if (Entities[OpponentEntityId].GetTagOrDefault(GameTag.CURRENT_PLAYER, 0) == 1)
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
