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
        public new Dictionary<int, Entity> Items
        {
            get { return items_; }
        }

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
    }
}
