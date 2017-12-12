using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HearthstoneAI.AI
{
    class AILogger
    {
        private System.Windows.Forms.TextBox textbox_;
        public AILogger(ref System.Windows.Forms.TextBox textbox)
        {
            textbox_ = textbox;
        }

        public void Info(System.String msg)
        {
            msg = msg.Replace("\n", "\r\n");
            LogMsg(DateTime.Now.ToShortTimeString() +
                " [INFO]" +
                " " + msg);
        }

        private void LogMsg(System.String msg)
        {
            textbox_.BeginInvoke(new System.Windows.Forms.MethodInvoker(() =>
            {
                if (textbox_.Text.Length > 0) textbox_.Text += System.Environment.NewLine;
                textbox_.Text += msg;
                textbox_.SelectionStart = textbox_.Text.Length;
                textbox_.ScrollToCaret();
            }));
        }
    }
}
