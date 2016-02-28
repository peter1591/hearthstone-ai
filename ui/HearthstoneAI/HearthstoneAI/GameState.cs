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

            public void SetTag(Tuple<GameTag, int> tag_value)
            {
                this.SetTag(tag_value.Item1, tag_value.Item2);
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
                        throw new Exception("Failed when parsing tag.");
                    }

                    if (Enum.IsDefined(typeof(GameTag), raw_tag_int))
                    {
                        tag = (GameTag) raw_tag_int;
                    }
                }

                var value = ParseTagValue(tag, raw_value);

                return new Tuple<GameTag, int>(tag, value);
            }

            private static int ParseTagValue(GameTag tag, string raw_value)
            {
                int value;
                switch (tag)
                {
                    case GameTag.ZONE:
                        TAG_ZONE zone;
                        Enum.TryParse(raw_value, out zone);
                        value = (int)zone;
                        break;

                    case GameTag.MULLIGAN_STATE:
                        {
                            TAG_MULLIGAN state;
                            Enum.TryParse(raw_value, out state);
                            value = (int)state;
                        }
                        break;
                    case GameTag.PLAYSTATE:
                        {
                            TAG_PLAYSTATE state;
                            Enum.TryParse(raw_value, out state);
                            value = (int)state;
                        }
                        break;
                    case GameTag.CARDTYPE:
                        TAG_CARDTYPE type;
                        Enum.TryParse(raw_value, out type);
                        value = (int)type;
                        break;
                    case GameTag.CLASS:
                        TAG_CLASS @class;
                        Enum.TryParse(raw_value, out @class);
                        value = (int)@class;
                        break;
                    default:
                        int.TryParse(raw_value, out value);
                        break;
                }
                return value;
            }
        }

        public class EntityChoice
        {
            public EntityChoice()
            {
                choices = new Dictionary<int, int>();
            }

            public int id;
            public string choice_type;
            public int player_entity_id;
            public string source;
            public Dictionary<int, int> choices;
        }

        public GameState()
        {
            this.Reset();
        }

        public void Reset()
        {
            this.Entities = new Dictionary<int, Entity>();
            this.EntityChoices = new Dictionary<int, EntityChoice>();
            this.GameEntityId = -1;
            this.PlayerEntityId = -1;
            this.OpponentEntityId = -1;
        }

        public Dictionary<int, Entity> Entities { get; set; }
        public Dictionary<int, EntityChoice> EntityChoices { get; set; }
        private int GameEntityId { get; set; }
        public int PlayerEntityId { get; set; }
        public int OpponentEntityId { get; set; }

        public void CreateGameEntity(int id)
        {
            if (Entities.ContainsKey(id))
            {
                // game entity has already been set, clearing all data
                this.Reset();
            }

            Entities.Add(id, new GameState.Entity(id) { Name = "GameEntity" });
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

        public void ChangeTag(int entity_id, string raw_tag, string raw_value)
        {
            if (!this.Entities.ContainsKey(entity_id))
            {
                this.Entities.Add(entity_id, new Entity(entity_id));
            }

            bool has_prev_value = this.Entities[entity_id].HasTag(GameTag.ZONE);
            int prev_value = -1;
            if (has_prev_value) prev_value = this.Entities[entity_id].GetTag(GameTag.ZONE);

            var tag_value = Entity.ParseTag(raw_tag, raw_value);
            this.Entities[entity_id].SetTag(tag_value);

            var tag = tag_value.Item1;
            var value = tag_value.Item2;

            switch (tag)
            {
                case GameTag.MULLIGAN_STATE:
                    this.MulliganStateChanged(entity_id, GameTag.MULLIGAN_STATE, has_prev_value, prev_value, value);
                    break;
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
    }
}
