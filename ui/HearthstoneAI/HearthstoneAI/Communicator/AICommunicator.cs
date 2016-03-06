using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;
using System.Runtime.Serialization.Json;

namespace HearthstoneAI.Communicator
{
    class AICommunicator
    {
        private static string ai_program_path = "./AI.exe";

        private Process process;
        private frmMain frm_main;
        private int next_action_sequence;
        private LinkedList<string> msg_stdout = new LinkedList<string>();
        private LinkedList<string> msg_stderr = new LinkedList<string>();

        private bool request_running = false;
        private DateTime last_request_start;
        private DateTime last_get_best_move;

        public AICommunicator(frmMain frm)
        {
            this.frm_main = frm;
            this.next_action_sequence = 1;
        }

        private void StandardOutputDataHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            if (!String.IsNullOrEmpty(outLine.Data)) this.msg_stdout.AddLast(outLine.Data);
        }

        private void StandardErrorDataHandler(object sendingProcess, DataReceivedEventArgs outLine)
        {
            if (!String.IsNullOrEmpty(outLine.Data)) this.msg_stderr.AddLast(outLine.Data);
        }

        private int GetNextActionSequenceId()
        {
            return this.next_action_sequence++;
        }

        public void Start()
        {
            this.process = new Process();
            this.process.StartInfo.FileName = ai_program_path;
            this.process.StartInfo.Arguments = "";
            this.process.StartInfo.CreateNoWindow = true; // TODO
            this.process.StartInfo.UseShellExecute = false;
            this.process.StartInfo.RedirectStandardInput = true;
            this.process.StartInfo.RedirectStandardOutput = true;
            this.process.StartInfo.RedirectStandardError = true;
            this.process.Start();

            this.process.OutputDataReceived += StandardOutputDataHandler;
            this.process.ErrorDataReceived += StandardErrorDataHandler;

            this.process.BeginOutputReadLine();
            this.process.BeginErrorReadLine();
        }

        public void HandleGameBoard(Board.Game game)
        {
            ActionGetBestMove action = new ActionGetBestMove(this.GetNextActionSequenceId(), game);
            this.SendRequest(action);

            this.request_running = true;
            this.last_request_start = DateTime.Now;
            this.last_get_best_move = DateTime.Now;
        }

        public void Cancel()
        {
            this.SendRequest(new ActionCancel(this.GetNextActionSequenceId()));
            this.request_running = false;
        }

        private void GetCurrentBestMove()
        {
            this.SendRequest(new ActionGetCurrentBestMove(this.GetNextActionSequenceId()));
            this.last_get_best_move = DateTime.Now;
        }

        private void SendRequest(ActionBase action)
        {
            MemoryStream stream = new MemoryStream();
            DataContractJsonSerializer serializer = new DataContractJsonSerializer(action.GetType());
            serializer.WriteObject(stream, action);
            stream.Position = 0;
            var sr = new StreamReader(stream);
            var json = sr.ReadToEnd();
            this.frm_main.AddLog("[INFO] Invoking AI with request: " + json);

            if (json.Contains('\r') || json.Contains('\n')) throw new Exception("request should not contains new line character");

            // TODO: cancel previous request if any

            process.StandardInput.Write(json);
            process.StandardInput.Write('\n');
        }

        public void Process()
        {
            while (this.msg_stderr.Count > 0)
            {
                this.frm_main.AddLog("[ERROR]: AI program produces stderr: " + this.msg_stderr.First.Value);
                this.msg_stderr.RemoveFirst();
            }

            while (this.msg_stdout.Count > 0)
            {
                this.frm_main.AddLog("[INFO]: AI program produces stdout: " + this.msg_stdout.First.Value);
                this.msg_stdout.RemoveFirst();
            }

            if (request_running)
            {
                if ((DateTime.Now - this.last_request_start) > new TimeSpan(0, 0, 10))
                {
                    this.GetCurrentBestMove();
                    this.Cancel();
                }
                else if ((DateTime.Now - this.last_get_best_move) > new TimeSpan(0,0,5))
                {
                    //this.GetCurrentBestMove();
                }
            }
        }
    }
}
