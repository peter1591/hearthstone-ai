using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.AI
{
    class AIEngine
    {
        public AILogger logger_;

        private GameEngineCppWrapper.CLI.GameEngine.OutputMessageCallback _output_message_cb;
        public GameEngineCppWrapper.CLI.GameEngine.OutputMessageCallback output_message_cb
        {
            get
            {
                return _output_message_cb;
            }
            set
            {
                _output_message_cb += value;
                engine_.SetOutputMessageCallback(_output_message_cb);
            }
        }

        public AIEngine(AILogger logger)
        {
            engine_ = new GameEngineCppWrapper.CLI.GameEngine();
            logger_ = logger;
        }

        private bool initialized_ = false;
        public int Initialize(int root_sample_count)
        {
            if (engine_.Initialize(root_sample_count) != 0) return -1;

            initialized_ = true;
            logger_.Info("Engine is now initialized.");
            return 0;
        }
        public bool IsInitialized() { return initialized_; }

        public bool IsRunning() {
            if (!IsInitialized()) return false;
            if (runner_ == null) return false;
            return runner_.IsAlive;
        }

        public int Reset()
        {
            AbortRunner();
            if (engine_.ResetBoard() < 0) return -1;
            return 0;
        }

        public int UpdateBoard(Board.Game board)
        {
            if (!IsInitialized())
            {
                logger_.Info("Engine is not initialized.");
                return -1;
            }

            if (IsRunning())
            {
                logger_.Info("Engine is running. Please stop the runner before updaing the board.");
                return -1;
            }

            System.Runtime.Serialization.Json.DataContractJsonSerializer ser =
                new System.Runtime.Serialization.Json.DataContractJsonSerializer(typeof(Board.Game));
            MemoryStream ms = new MemoryStream();

            ser.WriteObject(ms, board);
            ms.Position = 0;

            StreamReader sr = new StreamReader(ms);
            string json = sr.ReadToEnd();

            return engine_.UpdateBoard(json);
        }

        private DateTime last_run_start = DateTime.MaxValue;
        public int Run(int seconds, int threads)
        {
            if (!IsInitialized())
            {
                logger_.Info("Engine is not initialized.");
                return -1;
            }

            AbortRunner();

            runner_ = new System.Threading.Thread(() =>
            {
                logger_.Info(String.Format("Start run for {0} seconds with {1} threads.", seconds, threads));
                engine_.Run(seconds, threads);
                logger_.Info("Finish run.");
            });
            runner_.Start();
            last_run_start = DateTime.Now;

            return 0;
        }

        public int InteractiveShell(String cmd)
        {
            return engine_.InteractiveShell(cmd);
        }

        private TimeSpan min_time_to_get_best_choice = new TimeSpan(0, 0, 3);
        public String GetBestChoice()
        {
            if ((DateTime.Now - last_run_start) < min_time_to_get_best_choice)
            {
                return "(Preparing)";
            }

            return engine_.GetBestChoice();
        }

        public void Destroy()
        {
            AbortRunner();
            engine_ = null;
        }

        public void AbortRunner()
        {
            if (runner_ != null)
            {
                engine_.NotifyStop(); // request to early-stop
                while (IsRunning())
                {
                    System.Windows.Forms.Application.DoEvents();
                }
                runner_.Join();
                runner_ = null;
            }
        }

        private GameEngineCppWrapper.CLI.GameEngine engine_;
        private System.Threading.Thread runner_ = null;
    }
}
