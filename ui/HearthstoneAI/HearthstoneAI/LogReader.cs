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

        public LogReader(frmMain frm)
        {
            this.frmMain = frm;
            this.Reset();
        }

        private void Reset()
        {
            this.power_log_offset = 0;
            log_parser = new LogParser(this.frmMain);
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
            
            var lines = content.Split(new string[] { Environment.NewLine }, StringSplitOptions.None);
            foreach (string line in lines)
            {
                this.ProcessLog(line);
            }
        }

        private void ProcessLog(string log)
        {
            if (!log.StartsWith("D ")) return;

            LogItem log_item;

            try
            {
                log_item = LogItem.Parse(log);
            }
            catch (Exception ex)
            {
                return;
            }

            this.log_parser.Process(log_item);
        }

        public GameState GetGameState()
        {
            return this.log_parser.GameState;
        }
    }
}
