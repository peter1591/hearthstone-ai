using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher
{
    class StableDecider
    {
        private TimeSpan interval_;
        private DateTime last_stable_time_;

        public StableDecider(int interval_ms)
        {
            interval_ = new TimeSpan(0, 0, 0, 0, interval_ms);
            last_stable_time_ = DateTime.Now;
        }

        public void Changed()
        {
            last_stable_time_ = DateTime.Now;
        }

        public bool IsStable()
        {
            TimeSpan past = DateTime.Now - last_stable_time_;
            return past >= this.interval_;
        }
    }
}
