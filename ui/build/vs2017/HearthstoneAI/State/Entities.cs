using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.State
{
    class ReadOnlyEntities
    {
        protected ReadOnlyEntities()
        {
            items_ = new Dictionary<int, Entity>();
        }

        protected Dictionary<int, Entity> items_;

        public IReadOnlyDictionary<int, ReadOnlyEntity> Items
        {
            get
            {
                var d = new Dictionary<int, ReadOnlyEntity>();
                foreach (var kv in items_)
                {
                    d.Add(kv.Key, kv.Value);
                }
                return d;
            }
        }
    }

    class Entities : ReadOnlyEntities
    {
        public void Add(int id, Entity data)
        {
            items_.Add(id, data);
        }
        public void ChangeCardId(int id, string card_id)
        {
            items_[id].SetCardId(card_id);
        }
        public void ChangeName(int id, string name)
        {
            items_[id].SetName(name);
        }
        public void ChangeTag(int id, GameTag tag, int value)
        {
            TAG_ZONE prev_zone = (TAG_ZONE)items_[id].GetTagOrDefault(GameTag.ZONE, (int)TAG_ZONE.INVALID);

            items_[id].SetTag(tag, value);
        }
    }
}
