using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    public class ReadOnlyEntity
    {
        protected ReadOnlyEntity(int id)
        {
            Id = id;
            Tags = new Dictionary<GameTag, int>();
            Name = "";
            CardId = "";
        }

        public ReadOnlyEntity(ReadOnlyEntity rhs)
        {
            Id = rhs.Id;
            Tags = new Dictionary<GameTag, int>(rhs.Tags);
            Name = rhs.Name;
            CardId = rhs.CardId;
        }

        public ReadOnlyEntity Clone() { return new ReadOnlyEntity(this); }

        public int Id { get; }
        public Dictionary<GameTag, int> Tags { get; }
        public string Name { get; protected set; }
        public string CardId { get; protected set; }

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
    }

    public class Entity : ReadOnlyEntity
    {
        public Entity(int id) : base(id)
        {
        }

        public void SetName(string s) { Name = s; }
        public void SetCardId(string s) { CardId = s; }

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
}
