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
        public EntityGenerationInfo generate_under_blocks;

        public bool Parse(State.Game game, State.ReadOnlyEntity entity)
        {
            this.id = entity.Id;

            this.generate_under_blocks = new EntityGenerationInfo();
            foreach (var obj in entity.generate_under_blocks_)
            {
                this.generate_under_blocks.Add(obj);
            }

            return true;
        }

        public override bool Equals(object obj)
        {
            Entity rhs = obj as Entity;
            if (rhs == null) return false;
            if (!this.id.Equals(rhs.id)) return false;
            if (!this.generate_under_blocks.Equals(rhs.generate_under_blocks)) return false;
            return true;
        }

        public override int GetHashCode()
        {
            return new
            {
                this.id,
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
                Entity item = new Entity();
                if (!item.Parse(game, obj.Value)) return false;
                this.Add(item);
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
