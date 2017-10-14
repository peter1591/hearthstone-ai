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
        private State.Game game_state_;

        public event EventHandler<SubParsers.PowerLogParser.BlockStartEventArgs> BlockStart;
        public event EventHandler<SubParsers.PowerLogParser.BlockEndEventArgs> BlockEnd;
        public event EventHandler<SubParsers.PowerLogParser.EntityTagChangedEventArgs> EntityTagChanged;
        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;
        public event EventHandler<SubParsers.DebugPrintOptions.StartWaitingMainActionEventArgs> StartWaitingMainActionEvent;

        private Logger logger_;

        public delegate void LogChanged();
        public LogChanged log_changed;

        public LogReader(string hearthstone_path, State.Game game_state, Logger logger)
        {
            logger_ = logger;
            this.hearthstone_path = hearthstone_path;
            this.game_state_ = game_state;
            this.Reset();
        }

        private void Reset()
        {
            this.logger_.Info("LogReader is doing a (hard) reset.");
            this.power_log_offset = 0;
            this.game_state_.Reset();
            this.CreateLogParser();
            if (log_changed != null) log_changed();
        }

        private void CreateLogParser()
        {
            this.log_parser = new LogParser(game_state_, logger_);

            this.log_parser.BlockStart += (sender, e) =>
            {
                if (this.BlockStart != null) this.BlockStart(this, e);
            };
            this.log_parser.BlockEnd += (sender, e) => this.BlockEnd(sender, e);
            this.log_parser.EntityTagChanged += (sender, e) => this.EntityTagChanged(sender, e);
            this.log_parser.CreateGameEvent += (sender, e) =>
            {
                if (this.CreateGameEvent != null) this.CreateGameEvent(this, e);
            };
            this.log_parser.StartWaitingMainActionEvent += (sender, e) => this.StartWaitingMainActionEvent(sender, e);
        }

        private string GetPowerLogPath()
        {
            return Path.Combine(this.hearthstone_path, "Logs\\Power.log");
        }

        public void NewGameStart()
        {
            this.game_state_.Reset();
        }

        public void Process()
        {
            if (File.Exists(this.GetPowerLogPath()) == false)
            {
                this.logger_.Info("Log file does not exist.");
                return;
            }

            var power_log = new FileStream(this.GetPowerLogPath(), FileMode.Open, FileAccess.Read, FileShare.ReadWrite);

            if (power_log.Length == this.power_log_offset) return;

            if (power_log.Length < this.power_log_offset)
            {
                this.logger_.Info("Log file shrinked. Doing a reset.");
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
                //System.Windows.Forms.Application.DoEvents();
            }

            log_changed();
        }
    }
}
