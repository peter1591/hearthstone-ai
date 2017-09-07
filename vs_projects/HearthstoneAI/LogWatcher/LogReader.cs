using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.LogWatcher
{
    class LogReader
    {
        private string hearthstone_path;

        private long power_log_offset;
        private LogParser log_parser;

        private int change_id;

        public event EventHandler<GameState.StartWaitingMainActionEventArgs> StartWaitingMainAction;
        public event EventHandler<LogParser.ActionStartEventArgs> ActionStart;
        public event EventHandler<LogParser.EndTurnEventArgs> EndTurnEvent;
        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public delegate void LogMsgDelegate(String msg);
        public LogMsgDelegate log_msg;

        public LogReader(string hearthstone_path)
        {
            this.hearthstone_path = hearthstone_path;
            this.change_id = 1;
            this.Reset();
        }

        public int GetChangeId() { return this.change_id; }

        private void Reset()
        {
            this.power_log_offset = 0;
            this.CreateLogParser();
            this.change_id++;
        }

        private void CreateLogParser()
        {
            this.log_parser = new LogParser();
            this.log_parser.GameState.StartWaitingMainAction += (sender, e) =>
            {
                if (this.StartWaitingMainAction != null) this.StartWaitingMainAction(this, e);
            };

            this.log_parser.ActionStart += (sender, e) =>
            {
                if (this.ActionStart != null) this.ActionStart(this, e);
            };
            this.log_parser.CreateGameEvent += (sender, e) =>
            {
                if (this.CreateGameEvent != null) this.CreateGameEvent(this, e);
            };

            this.log_parser.EndTurnEvent += (sender, e) =>
            {
                if (this.EndTurnEvent != null) this.EndTurnEvent(this, e);
            };

            this.log_parser.log_msg += (string msg) => log_msg(msg);
        }

        private string GetPowerLogPath()
        {
            return Path.Combine(this.hearthstone_path, "Logs\\Power.log");
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
                System.Windows.Forms.Application.DoEvents();
            }

            this.change_id++;
        }

        public GameState GetGameState()
        {
            return this.log_parser.GameState;
        }
    }
}
