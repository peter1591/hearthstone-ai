using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI
{
    class LogReader
    {
        public frmMain frmMain { get; set; }

        private long power_log_offset;
        private LogParser log_parser;

        private int change_id;

        public LogReader(frmMain frm)
        {
            this.change_id = 1;
            this.frmMain = frm;
            this.Reset();
        }

        public int GetChangeId() { return this.change_id; }

        private void Reset()
        {
            this.power_log_offset = 0;
            log_parser = new LogParser(this.frmMain);
            this.change_id++;
        }

        private string GetPowerLogPath()
        {
            string installation_path = this.frmMain.HearthstoneInstallationPath;
            return Path.Combine(installation_path, "Logs\\Power.log");
        }

        public void Process()
        {
            if (File.Exists(this.GetPowerLogPath()) == false) return;

            var power_log = new FileStream(this.GetPowerLogPath(), FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

            if (power_log.Length == this.power_log_offset) return;

            if (power_log.Length < this.power_log_offset)
            {
                // log shrinked -> reload
                this.Reset();
            }

            power_log.Seek(power_log_offset, SeekOrigin.Begin);
            var power_log_stream = new StreamReader(power_log);

            if (power_log_stream.EndOfStream) return;

            string content_all = power_log_stream.ReadToEnd();

            // Note: if the last character is not the newline character
            // it means someone is still writing to the last line
            // --> skip the last line
            int content_end = content_all.LastIndexOfAny(Environment.NewLine.ToCharArray());
            string content = content_all.Substring(0, content_end + 1);
            this.power_log_offset += Encoding.UTF8.GetByteCount(content);

            var new_log_lines = content.Split(Environment.NewLine.ToCharArray(), StringSplitOptions.None);

            foreach (var new_log_line in new_log_lines)
            {
                this.log_parser.Process(new_log_line);
            }

            this.change_id++;
        }

        public GameState GetGameState()
        {
            return this.log_parser.GameState;
        }
    }
}
