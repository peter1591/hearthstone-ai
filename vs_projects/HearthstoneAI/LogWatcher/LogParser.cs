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

        public LogParser(frmMain frm)
        {
            this.frmMain = frm;
            this.GameState = new GameState();

            this.power_log_parser = new SubParsers.PowerLogParser(this.frmMain, this.GameState);
            this.entity_choices_parser = new SubParsers.EntityChoicesParser(this.frmMain, this.GameState);
            this.send_choices_parser = new SubParsers.SendChoicesParser(this.frmMain, this.GameState);
            this.debug_print_power_parser = new SubParsers.DebugPrintPowerParser(this.frmMain, this.GameState);

            this.power_log_parser.ActionStart += (sender, e) =>
            {
                if (this.ActionStart != null) this.ActionStart(this, new ActionStartEventArgs(e, this.GameState));
            };
            this.power_log_parser.CreateGameEvent += (sender, e) =>
            {
                if (this.CreateGameEvent != null) this.CreateGameEvent(this, e);
            };
            this.GameState.EndTurnEvent += (sender, e) =>
            {
                if (this.EndTurnEvent != null) this.EndTurnEvent(this, new EndTurnEventArgs(e, this.GameState));
            };
        }

        private frmMain frmMain;
        private SubParsers.PowerLogParser power_log_parser;
        private SubParsers.SendChoicesParser send_choices_parser;
        private SubParsers.EntityChoicesParser entity_choices_parser;
        private SubParsers.DebugPrintPowerParser debug_print_power_parser;

        public GameState GameState { get; }

        public class ActionStartEventArgs : SubParsers.PowerLogParser.ActionStartEventArgs
        {
            public ActionStartEventArgs(SubParsers.PowerLogParser.ActionStartEventArgs e, GameState game) : base(e)
            {
                this.game = game;
            }

            public GameState game;
        };
        public event EventHandler<ActionStartEventArgs> ActionStart;

        public class EndTurnEventArgs : GameState.EndTurnEventArgs
        {
            public EndTurnEventArgs(GameState.EndTurnEventArgs e, GameState game) : base(e)
            {
                this.game = game;
            }

            public GameState game;
        };
        public event EventHandler<EndTurnEventArgs> EndTurnEvent;

        public event EventHandler<SubParsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

        public void Process(string log_line)
        {
            if (log_line == "") return;

            LogItem log_item;

            try { log_item = LogItem.Parse(log_line); }
            catch (Exception)
            {
                this.frmMain.AddLog("Failed when parsing: " + log_line);
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
                //this.frmMain.AddLog("Failed when parsing: " + log_line);
            }
        }
    }
}
