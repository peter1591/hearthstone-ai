using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI.LogWatcher
{
    class LogParser
    {
        private static string PowerLogPrefix = "GameState.DebugPrintPower() - ";
        private static string EntityChoicesLogPrefix = "GameState.DebugPrintEntityChoices() - ";
        private static string SendChoicesLogPrefix = "GameState.SendChoices() - ";
        private static string PowerTaskListDebugDumpLogPrefix = "PowerTaskList.DebugDump() - ";
        private static string PowerTaskListDebugPrintPowerLogPrefix = "PowerTaskList.DebugPrintPower() - ";

        private GameState game_state_;
        public LogParser(GameState game_state, Logger logger)
        {
            logger_ = logger;
            game_state_ = game_state;

            this.power_log_parser = new SubParsers.PowerLogParser(game_state_, logger_);
            this.entity_choices_parser = new SubParsers.EntityChoicesParser(game_state_, logger_);
            this.send_choices_parser = new SubParsers.SendChoicesParser(game_state_, logger_);
            this.debug_print_power_parser = new SubParsers.DebugPrintPowerParser(game_state_, logger_);

            this.power_log_parser.ActionStart += (sender, e) =>
            {
                if (this.ActionStart != null) this.ActionStart(this, e);
            };
            this.power_log_parser.CreateGameEvent += (sender, e) =>
            {
                if (this.CreateGameEvent != null) this.CreateGameEvent(this, e);
            };
        }

        private SubParsers.PowerLogParser power_log_parser;
        private SubParsers.SendChoicesParser send_choices_parser;
        private SubParsers.EntityChoicesParser entity_choices_parser;
        private SubParsers.DebugPrintPowerParser debug_print_power_parser;

        private Logger logger_;

        public event EventHandler<SubParsers.PowerLogParser.ActionStartEventArgs> ActionStart;
        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public void Process(string log_line)
        {
            if (log_line == "") return;

            LogItem log_item;

            try { log_item = LogItem.Parse(log_line); }
            catch (Exception)
            {
                logger_.Info("Failed when parsing: " + log_line);
                return;
            }

            string log = log_item.Content;

            if (log.StartsWith(PowerLogPrefix))
            {
                this.power_log_parser.Process(log.Substring(PowerLogPrefix.Length));
            }
            else if (log.StartsWith(EntityChoicesLogPrefix))
            {
                entity_choices_parser.Process(log.Substring(EntityChoicesLogPrefix.Length));
            }
            else if (log.StartsWith(SendChoicesLogPrefix))
            {
                send_choices_parser.Process(log.Substring(SendChoicesLogPrefix.Length));
            }
            else if (log.StartsWith(PowerTaskListDebugDumpLogPrefix)) { }
            else if (log.StartsWith(PowerTaskListDebugPrintPowerLogPrefix))
            {
                debug_print_power_parser.Process(log.Substring(PowerTaskListDebugPrintPowerLogPrefix.Length));
            }
            else
            {
                // continue
            }
        }
    }
}
