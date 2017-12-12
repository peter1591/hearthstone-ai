using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher.SubParsers
{
    class BaseParser
    {
        public BaseParser(Logger logger)
        {
            logger_ = logger;
        }

        protected Logger logger_;
    }
}
