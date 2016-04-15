using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Text.RegularExpressions;

namespace HearthstoneAI
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

            this.power_log_parser = new Parsers.PowerLogParser(this.frmMain, this.GameState);
            this.entity_choices_parser = new Parsers.EntityChoicesParser(this.frmMain, this.GameState);
            this.send_choices_parser = new Parsers.SendChoicesParser(this.frmMain, this.GameState);

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
        private Parsers.PowerLogParser power_log_parser;
        private Parsers.SendChoicesParser send_choices_parser;
        private Parsers.EntityChoicesParser entity_choices_parser;

        public GameState GameState { get; }

        public class ActionStartEventArgs : Parsers.PowerLogParser.ActionStartEventArgs
        {
            public ActionStartEventArgs(Parsers.PowerLogParser.ActionStartEventArgs e, GameState game) : base(e)
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

        public event EventHandler<Parsers.PowerLogParser.CreateGameEventArgs> CreateGameEvent;

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
            else if (log.StartsWith(PowerTaskListDebugPrintPowerLogPrefix)) { }
            else
            {
                //this.frmMain.AddLog("Failed when parsing: " + log_line);
            }
        }
    }
}
