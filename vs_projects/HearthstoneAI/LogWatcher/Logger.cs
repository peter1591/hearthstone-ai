using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher
{
    class Logger
    {
        public delegate void Callback(String msg);

        Callback info_cb_;

        public void RegisterInfoCallback(Callback cb)
        {
            info_cb_ += cb;
        }

        public void Info(String msg)
        {
            if (info_cb_ == null) return;
            info_cb_(msg);
        }
    }
}
