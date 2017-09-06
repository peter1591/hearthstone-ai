using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI
{
    class LogItem
    {
        private LogItem(DateTime time, string content)
        {
            Time = time;
            Content = content;
        }

        public static LogItem Parse(string line)
        {
            if (!line.StartsWith("D ")) throw new Exception("wrong format");

            DateTime time;

            if (line.Length < 19) throw new Exception("wrong format");

            if (DateTime.TryParse(line.Substring(2, 16), out time) == false)
            {
                throw new Exception("wrong format");
            }

            string content = line.Substring(19);

            return new LogItem(time, content);
        }

        public DateTime Time { get; }
        public string Content { get; set; }

        public override string ToString()
        {
            return "[" + Time.ToLongTimeString() + "] " + Content;
        }
    }
}
