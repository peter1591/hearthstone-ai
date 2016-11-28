using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.Board
{
    class HashHelper
    {
        public static int init = 17;
        private static int multiplier = 23;

        public static void Update<T>(ref int hash, T obj)
        {
            hash = hash * multiplier + obj.GetHashCode();
        }
    }
}
