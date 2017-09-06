using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameEngineUI
{
    class Logger
    {
        private System.Windows.Forms.TextBox textbox_;
        public Logger(ref System.Windows.Forms.TextBox textbox)
        {
            textbox_ = textbox;
        }

        public void Info(System.String msg)
        {
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
