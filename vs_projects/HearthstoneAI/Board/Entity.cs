using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Serialization;

namespace HearthstoneAI.Board
{
    class EntityGenerationInfo : List<int>
    {
        public override bool Equals(object obj)
        {
            EntityGenerationInfo rhs = obj as EntityGenerationInfo;
            if (rhs == null) return false;
            return this.SequenceEqual(rhs);
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            foreach (var obj in this)
                HashHelper.Update(ref hash, obj);
            return hash;
        }
    }

    [DataContract]
    class Entity
    {
        [DataMember]
        public int id;

        [DataMember]
        public string card_id;

        [DataMember]
        public int controller;

        [DataMember]
        public EntityGenerationInfo generate_under_blocks;

        [DataMember]
        public Dictionary<string, int> tags;

        public bool Parse(State.Game game, State.ReadOnlyEntity entity)
        {
            this.id = entity.Id;

            this.card_id = entity.CardId;
            this.controller = entity.GetTagOrDefault(State.GameTag.CONTROLLER, -1);

            this.generate_under_blocks = new EntityGenerationInfo();
            foreach (var obj in entity.generate_under_blocks_)
            {
                this.generate_under_blocks.Add(obj);
            }

            tags = new Dictionary<string, int>();
            foreach (var kv in entity.Tags)
            {
                tags[kv.Key.ToString()] = kv.Value;
            }

            return true;
        }

        public override bool Equals(object obj)
        {
            Entity rhs = obj as Entity;
            if (rhs == null) return false;
            if (!this.id.Equals(rhs.id)) return false;
            if (!this.card_id.Equals(rhs.card_id)) return false;
            if (!this.controller.Equals(rhs.controller)) return false;
            if (!this.generate_under_blocks.Equals(rhs.generate_under_blocks)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.id,
                this.card_id,
                this.controller,
                this.generate_under_blocks
            }.GetHashCode();
        }
    }

    class Entities : List<Entity>
    {
        public bool Parse(State.Game game)
        {
            foreach (var obj in game.Entities.Items)
            {
                while (obj.Key >= this.Count) this.Add(new Entity());

                if (!this[obj.Key].Parse(game, obj.Value)) return false;
            }

            return true;
        }

        public override bool Equals(object obj)
        {
            Entities rhs = obj as Entities;
            if (rhs == null) return false;
            return this.SequenceEqual(rhs);
        }

        public override int GetHashCode()
        {
            int hash = HashHelper.init;
            foreach (var obj in this)
                HashHelper.Update(ref hash, obj);
            return hash;
        }
    }
}
