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
        }

        private frmMain frmMain;

        public GameState GameState { get; }

        public string[] new_log_lines; // for Process()

        // return true if still parsing; false if all lines are parsed
        // never break
        public IEnumerable<bool> Process()
        {
            Parsers.PowerLogParser power_log_parser = new Parsers.PowerLogParser(this.frmMain, this.GameState);
            Parsers.SendChoicesParser send_choices_parser = new Parsers.SendChoicesParser(this.frmMain, this.GameState);
            Parsers.EntityChoicesParser entity_choices_parser = new Parsers.EntityChoicesParser(this.frmMain, this.GameState);

            IEnumerator<bool> power_log = power_log_parser.Process().GetEnumerator();
            IEnumerator<bool> entity_choices_log = entity_choices_parser.Process().GetEnumerator();
            IEnumerator<bool> send_choices_log = send_choices_parser.Process().GetEnumerator();

            while (true)
            {
                foreach (var log_line in this.new_log_lines)
                {
                    if (log_line == "") continue;

                    LogItem log_item;

                    try { log_item = LogItem.Parse(log_line); }
                    catch (Exception ex)
                    {
                        this.frmMain.AddLog("Failed when parsing: " + log_line);
                        continue;
                    }

                    string log = log_item.Content;

                    if (log.StartsWith(PowerLogPrefix))
                    {
                        power_log_parser.parsing_log = log.Substring(PowerLogPrefix.Length);
                        power_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(EntityChoicesLogPrefix))
                    {
                        entity_choices_parser.parsing_log = log.Substring(EntityChoicesLogPrefix.Length);
                        entity_choices_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(SendChoicesLogPrefix))
                    {
                        send_choices_parser.parsing_log = log.Substring(SendChoicesLogPrefix.Length);
                        send_choices_log.MoveNext();
                        yield return true;
                    }
                    else if (log.StartsWith(PowerTaskListDebugDumpLogPrefix)) { }
                    else if (log.StartsWith(PowerTaskListDebugPrintPowerLogPrefix)) { }
                    else
                    {
                        //this.frmMain.AddLog("Failed when parsing: " + log_line);
                    }
                }

                yield return false;
            }
        }
    }
}
