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

        public AIEngine(AILogger logger, int seconds, int threads)
        {
            engine_ = new GameEngineCppWrapper.CLI.GameEngine();
            logger_ = logger;
            seconds_ = seconds;
            threads_ = threads;
        }

        private bool initialized_ = false;
        public int Initialize()
        {
            if (engine_.Initialize() != 0) return -1;

            initialized_ = true;
            logger_.Info("Engine is now initialized.");
            return 0;
        }
        public bool IsInitialized() { return initialized_; }

        public bool IsRunning() {
            if (!IsInitialized()) return false;
            if (runner_ != null) return false;
            return runner_.IsAlive;
        }

        public int Reset()
        {
            // TODO: implement
            logger_.Info("Reset game");
            return -1;
        }

        public int UpdateBoard(Board.Game board)
        {
            if (!IsInitialized())
            {
                logger_.Info("Engine is not initialized.");
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

        private int seconds_;
        private int threads_;
        public int Run()
        {
            if (!IsInitialized())
            {
                logger_.Info("Engine is not initialized.");
                return -1;
            }

            AbortRunner();

            runner_ = new System.Threading.Thread(() =>
            {
                logger_.Info(String.Format("Start run for {0} seconds with {1} threads.", seconds_, threads_));
                engine_.Run(seconds_, threads_);
                logger_.Info("Finish run.");
            });
            runner_.Start();

            return 0;
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
                engine_.Stop(); // request to early-stop
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
