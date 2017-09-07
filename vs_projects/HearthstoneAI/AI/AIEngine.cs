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
        public void Initialize()
        {
            new System.Threading.Thread(() =>
            {
                if (engine_.Initialize() != 0)
                {
                    logger_.Info("Failed to initialize.");
                    return;
                }
                initialized_ = true;
                logger_.Info("Engine initialized.");
            }).Start();
        }
        public bool IsInitialized() { return initialized_; }

        public bool IsRunning() { return (runner_ != null); }

        public int Run(int seconds, int threads)
        {
            if (!IsInitialized())
            {
                logger_.Info("Logger is not initialized.");
                return -1;
            }

            if (runner_ != null)
            {
                if (runner_.IsAlive)
                {
                    logger_.Info("Another runner thread is still running.");
                    return -1;
                }
            }

            runner_ = new System.Threading.Thread(() =>
            {
                logger_.Info(String.Format("Start run for {0} seconds with {1} threads.",
                    seconds, threads));
                engine_.Run(seconds, threads);
                logger_.Info("Finish run.");
            });
            runner_.Start();

            return 0;
        }

        public int Reset()
        {
            // TODO: implement
            logger_.Info("Reset game");
            return -1;
        }

        public int UpdateBoard(Board.Game board)
        {
            // TODO: implement
            logger_.Info("Update board");
            return -1;
        }

        public int MakeChoice(Board.Game board)
        {
            System.Runtime.Serialization.Json.DataContractJsonSerializer ser =
                new System.Runtime.Serialization.Json.DataContractJsonSerializer(typeof(Board.Game));
            MemoryStream ms = new MemoryStream();

            ser.WriteObject(ms, board);
            ms.Position = 0;

            StreamReader sr = new StreamReader(ms);
            string json = sr.ReadToEnd();

            // TODO: implement
            logger_.Info("Make choice! " + json);
            return -1;
        }

        public void Destroy()
        {
            if (runner_ != null && runner_.IsAlive)
            {
                // TODO: notify engine_ to early-stop
                runner_.Join();
            }

            engine_ = null;
        }

        private GameEngineCppWrapper.CLI.GameEngine engine_;
        private System.Threading.Thread runner_ = null;
    }
}
