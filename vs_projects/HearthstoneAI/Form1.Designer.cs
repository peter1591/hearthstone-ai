namespace HearthstoneAI
{
    partial class frmMain
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPageSettings = new System.Windows.Forms.TabPage();
            this.label17 = new System.Windows.Forms.Label();
            this.nudRootSampleCount = new System.Windows.Forms.NumericUpDown();
            this.label16 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.nudThreads = new System.Windows.Forms.NumericUpDown();
            this.nudSeconds = new System.Windows.Forms.NumericUpDown();
            this.btnStart = new System.Windows.Forms.Button();
            this.btnChangeHearthstoneInstallationPath = new System.Windows.Forms.Button();
            this.txtHearthstoneInstallationPath = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.listBoxProcessedLogs = new System.Windows.Forms.ListBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.label14 = new System.Windows.Forms.Label();
            this.txtChoices = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.txtOpponentMinions = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.txtOpponentSecrets = new System.Windows.Forms.TextBox();
            this.label11 = new System.Windows.Forms.Label();
            this.txtPlayerSecrets = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.txtPlayerMinions = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.txtOpponentDeck = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.txtPlayerDeck = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.txtOpponentHand = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.txtPlayerHand = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.txtOpponentHero = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.txtPlayerHero = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.txtGameEntity = new System.Windows.Forms.TextBox();
            this.tabPageAI = new System.Windows.Forms.TabPage();
            this.txtAIShell = new System.Windows.Forms.TextBox();
            this.label18 = new System.Windows.Forms.Label();
            this.txtAIEngine = new System.Windows.Forms.TextBox();
            this.timerMainLoop = new System.Windows.Forms.Timer(this.components);
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.tabControl1.SuspendLayout();
            this.tabPageSettings.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudRootSampleCount)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudThreads)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSeconds)).BeginInit();
            this.tabPage2.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPageAI.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPageSettings);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPageAI);
            this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Margin = new System.Windows.Forms.Padding(6);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(1745, 1458);
            this.tabControl1.TabIndex = 0;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            this.tabControl1.TabIndexChanged += new System.EventHandler(this.tabControl1_TabIndexChanged);
            // 
            // tabPageSettings
            // 
            this.tabPageSettings.Controls.Add(this.label17);
            this.tabPageSettings.Controls.Add(this.nudRootSampleCount);
            this.tabPageSettings.Controls.Add(this.label16);
            this.tabPageSettings.Controls.Add(this.label15);
            this.tabPageSettings.Controls.Add(this.nudThreads);
            this.tabPageSettings.Controls.Add(this.nudSeconds);
            this.tabPageSettings.Controls.Add(this.btnStart);
            this.tabPageSettings.Controls.Add(this.btnChangeHearthstoneInstallationPath);
            this.tabPageSettings.Controls.Add(this.txtHearthstoneInstallationPath);
            this.tabPageSettings.Controls.Add(this.label2);
            this.tabPageSettings.Location = new System.Drawing.Point(4, 33);
            this.tabPageSettings.Margin = new System.Windows.Forms.Padding(6);
            this.tabPageSettings.Name = "tabPageSettings";
            this.tabPageSettings.Padding = new System.Windows.Forms.Padding(6);
            this.tabPageSettings.Size = new System.Drawing.Size(1737, 1421);
            this.tabPageSettings.TabIndex = 0;
            this.tabPageSettings.Text = "Settings";
            this.tabPageSettings.UseVisualStyleBackColor = true;
            this.tabPageSettings.Click += new System.EventHandler(this.tabPageSettings_Click);
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Location = new System.Drawing.Point(11, 166);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(179, 25);
            this.label17.TabIndex = 7;
            this.label17.Text = "Root sample count:";
            // 
            // nudRootSampleCount
            // 
            this.nudRootSampleCount.Location = new System.Drawing.Point(196, 164);
            this.nudRootSampleCount.Maximum = new decimal(new int[] {
            10000,
            0,
            0,
            0});
            this.nudRootSampleCount.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudRootSampleCount.Name = "nudRootSampleCount";
            this.nudRootSampleCount.Size = new System.Drawing.Size(120, 29);
            this.nudRootSampleCount.TabIndex = 6;
            this.nudRootSampleCount.Value = new decimal(new int[] {
            30,
            0,
            0,
            0});
            this.nudRootSampleCount.ValueChanged += new System.EventHandler(this.numericUpDown1_ValueChanged);
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Location = new System.Drawing.Point(11, 131);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(91, 25);
            this.label16.TabIndex = 5;
            this.label16.Text = "Threads:";
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Location = new System.Drawing.Point(11, 96);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(96, 25);
            this.label15.TabIndex = 5;
            this.label15.Text = "Seconds:";
            // 
            // nudThreads
            // 
            this.nudThreads.Location = new System.Drawing.Point(113, 129);
            this.nudThreads.Maximum = new decimal(new int[] {
            32,
            0,
            0,
            0});
            this.nudThreads.Minimum = new decimal(new int[] {
            1,
            0,
            0,
            0});
            this.nudThreads.Name = "nudThreads";
            this.nudThreads.Size = new System.Drawing.Size(120, 29);
            this.nudThreads.TabIndex = 4;
            this.nudThreads.Value = new decimal(new int[] {
            1,
            0,
            0,
            0});
            // 
            // nudSeconds
            // 
            this.nudSeconds.Location = new System.Drawing.Point(113, 94);
            this.nudSeconds.Maximum = new decimal(new int[] {
            999,
            0,
            0,
            0});
            this.nudSeconds.Name = "nudSeconds";
            this.nudSeconds.Size = new System.Drawing.Size(120, 29);
            this.nudSeconds.TabIndex = 4;
            this.nudSeconds.Value = new decimal(new int[] {
            60,
            0,
            0,
            0});
            // 
            // btnStart
            // 
            this.btnStart.Location = new System.Drawing.Point(16, 213);
            this.btnStart.Margin = new System.Windows.Forms.Padding(6);
            this.btnStart.Name = "btnStart";
            this.btnStart.Size = new System.Drawing.Size(138, 42);
            this.btnStart.TabIndex = 3;
            this.btnStart.Text = "Start";
            this.btnStart.UseVisualStyleBackColor = true;
            this.btnStart.Click += new System.EventHandler(this.btnStart_Click);
            // 
            // btnChangeHearthstoneInstallationPath
            // 
            this.btnChangeHearthstoneInstallationPath.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnChangeHearthstoneInstallationPath.Location = new System.Drawing.Point(1914, 15);
            this.btnChangeHearthstoneInstallationPath.Margin = new System.Windows.Forms.Padding(6);
            this.btnChangeHearthstoneInstallationPath.Name = "btnChangeHearthstoneInstallationPath";
            this.btnChangeHearthstoneInstallationPath.Size = new System.Drawing.Size(138, 42);
            this.btnChangeHearthstoneInstallationPath.TabIndex = 2;
            this.btnChangeHearthstoneInstallationPath.Text = "Change";
            this.btnChangeHearthstoneInstallationPath.UseVisualStyleBackColor = true;
            this.btnChangeHearthstoneInstallationPath.Click += new System.EventHandler(this.btnChangeHearthstoneInstallationPath_Click);
            // 
            // txtHearthstoneInstallationPath
            // 
            this.txtHearthstoneInstallationPath.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtHearthstoneInstallationPath.Location = new System.Drawing.Point(290, 18);
            this.txtHearthstoneInstallationPath.Margin = new System.Windows.Forms.Padding(6);
            this.txtHearthstoneInstallationPath.Name = "txtHearthstoneInstallationPath";
            this.txtHearthstoneInstallationPath.Size = new System.Drawing.Size(1610, 29);
            this.txtHearthstoneInstallationPath.TabIndex = 1;
            this.txtHearthstoneInstallationPath.Text = "C:\\Program Files (x86)\\Hearthstone";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(11, 24);
            this.label2.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(266, 25);
            this.label2.TabIndex = 0;
            this.label2.Text = "Hearthstone Installation Path:";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.listBoxProcessedLogs);
            this.tabPage2.Controls.Add(this.label1);
            this.tabPage2.Location = new System.Drawing.Point(4, 33);
            this.tabPage2.Margin = new System.Windows.Forms.Padding(6);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(6);
            this.tabPage2.Size = new System.Drawing.Size(1737, 1421);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Log";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // listBoxProcessedLogs
            // 
            this.listBoxProcessedLogs.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listBoxProcessedLogs.FormattingEnabled = true;
            this.listBoxProcessedLogs.HorizontalScrollbar = true;
            this.listBoxProcessedLogs.ItemHeight = 24;
            this.listBoxProcessedLogs.Location = new System.Drawing.Point(20, 52);
            this.listBoxProcessedLogs.Margin = new System.Windows.Forms.Padding(6);
            this.listBoxProcessedLogs.Name = "listBoxProcessedLogs";
            this.listBoxProcessedLogs.Size = new System.Drawing.Size(1693, 1348);
            this.listBoxProcessedLogs.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 22);
            this.label1.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(159, 25);
            this.label1.TabIndex = 0;
            this.label1.Text = "Processed Logs:";
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.label14);
            this.tabPage1.Controls.Add(this.txtChoices);
            this.tabPage1.Controls.Add(this.label13);
            this.tabPage1.Controls.Add(this.txtOpponentMinions);
            this.tabPage1.Controls.Add(this.label12);
            this.tabPage1.Controls.Add(this.txtOpponentSecrets);
            this.tabPage1.Controls.Add(this.label11);
            this.tabPage1.Controls.Add(this.txtPlayerSecrets);
            this.tabPage1.Controls.Add(this.label10);
            this.tabPage1.Controls.Add(this.txtPlayerMinions);
            this.tabPage1.Controls.Add(this.label9);
            this.tabPage1.Controls.Add(this.txtOpponentDeck);
            this.tabPage1.Controls.Add(this.label8);
            this.tabPage1.Controls.Add(this.txtPlayerDeck);
            this.tabPage1.Controls.Add(this.label7);
            this.tabPage1.Controls.Add(this.txtOpponentHand);
            this.tabPage1.Controls.Add(this.label6);
            this.tabPage1.Controls.Add(this.txtPlayerHand);
            this.tabPage1.Controls.Add(this.label5);
            this.tabPage1.Controls.Add(this.txtOpponentHero);
            this.tabPage1.Controls.Add(this.label4);
            this.tabPage1.Controls.Add(this.txtPlayerHero);
            this.tabPage1.Controls.Add(this.label3);
            this.tabPage1.Controls.Add(this.txtGameEntity);
            this.tabPage1.Location = new System.Drawing.Point(4, 33);
            this.tabPage1.Margin = new System.Windows.Forms.Padding(6);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(6);
            this.tabPage1.Size = new System.Drawing.Size(1737, 1421);
            this.tabPage1.TabIndex = 2;
            this.tabPage1.Text = "Board";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(15, 474);
            this.label14.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(84, 25);
            this.label14.TabIndex = 23;
            this.label14.Text = "Choices";
            // 
            // txtChoices
            // 
            this.txtChoices.Location = new System.Drawing.Point(15, 504);
            this.txtChoices.Margin = new System.Windows.Forms.Padding(6);
            this.txtChoices.Multiline = true;
            this.txtChoices.Name = "txtChoices";
            this.txtChoices.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtChoices.Size = new System.Drawing.Size(325, 443);
            this.txtChoices.TabIndex = 22;
            this.txtChoices.WordWrap = false;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(1038, 26);
            this.label13.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(172, 25);
            this.label13.TabIndex = 21;
            this.label13.Text = "Opponent Minions";
            // 
            // txtOpponentMinions
            // 
            this.txtOpponentMinions.Location = new System.Drawing.Point(1038, 55);
            this.txtOpponentMinions.Margin = new System.Windows.Forms.Padding(6);
            this.txtOpponentMinions.Multiline = true;
            this.txtOpponentMinions.Name = "txtOpponentMinions";
            this.txtOpponentMinions.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtOpponentMinions.Size = new System.Drawing.Size(325, 1337);
            this.txtOpponentMinions.TabIndex = 20;
            this.txtOpponentMinions.WordWrap = false;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(1366, 419);
            this.label12.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(171, 25);
            this.label12.TabIndex = 19;
            this.label12.Text = "Opponent Secrets";
            // 
            // txtOpponentSecrets
            // 
            this.txtOpponentSecrets.Location = new System.Drawing.Point(1377, 449);
            this.txtOpponentSecrets.Margin = new System.Windows.Forms.Padding(6);
            this.txtOpponentSecrets.Multiline = true;
            this.txtOpponentSecrets.Name = "txtOpponentSecrets";
            this.txtOpponentSecrets.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtOpponentSecrets.Size = new System.Drawing.Size(314, 279);
            this.txtOpponentSecrets.TabIndex = 18;
            this.txtOpponentSecrets.WordWrap = false;
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(348, 415);
            this.label11.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(139, 25);
            this.label11.TabIndex = 17;
            this.label11.Text = "Player Secrets";
            // 
            // txtPlayerSecrets
            // 
            this.txtPlayerSecrets.Location = new System.Drawing.Point(359, 445);
            this.txtPlayerSecrets.Margin = new System.Windows.Forms.Padding(6);
            this.txtPlayerSecrets.Multiline = true;
            this.txtPlayerSecrets.Name = "txtPlayerSecrets";
            this.txtPlayerSecrets.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtPlayerSecrets.Size = new System.Drawing.Size(314, 279);
            this.txtPlayerSecrets.TabIndex = 16;
            this.txtPlayerSecrets.WordWrap = false;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(699, 26);
            this.label10.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(140, 25);
            this.label10.TabIndex = 15;
            this.label10.Text = "Player Minions";
            // 
            // txtPlayerMinions
            // 
            this.txtPlayerMinions.Location = new System.Drawing.Point(699, 55);
            this.txtPlayerMinions.Margin = new System.Windows.Forms.Padding(6);
            this.txtPlayerMinions.Multiline = true;
            this.txtPlayerMinions.Name = "txtPlayerMinions";
            this.txtPlayerMinions.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtPlayerMinions.Size = new System.Drawing.Size(325, 1337);
            this.txtPlayerMinions.TabIndex = 14;
            this.txtPlayerMinions.WordWrap = false;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(1377, 1091);
            this.label9.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(149, 25);
            this.label9.TabIndex = 13;
            this.label9.Text = "Opponent Deck";
            // 
            // txtOpponentDeck
            // 
            this.txtOpponentDeck.Location = new System.Drawing.Point(1377, 1121);
            this.txtOpponentDeck.Margin = new System.Windows.Forms.Padding(6);
            this.txtOpponentDeck.Multiline = true;
            this.txtOpponentDeck.Name = "txtOpponentDeck";
            this.txtOpponentDeck.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtOpponentDeck.Size = new System.Drawing.Size(325, 275);
            this.txtOpponentDeck.TabIndex = 12;
            this.txtOpponentDeck.WordWrap = false;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(348, 1087);
            this.label8.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(117, 25);
            this.label8.TabIndex = 11;
            this.label8.Text = "Player Deck";
            // 
            // txtPlayerDeck
            // 
            this.txtPlayerDeck.Location = new System.Drawing.Point(354, 1117);
            this.txtPlayerDeck.Margin = new System.Windows.Forms.Padding(6);
            this.txtPlayerDeck.Multiline = true;
            this.txtPlayerDeck.Name = "txtPlayerDeck";
            this.txtPlayerDeck.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtPlayerDeck.Size = new System.Drawing.Size(325, 275);
            this.txtPlayerDeck.TabIndex = 10;
            this.txtPlayerDeck.WordWrap = false;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(1377, 746);
            this.label7.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(151, 25);
            this.label7.TabIndex = 9;
            this.label7.Text = "Opponent Hand";
            // 
            // txtOpponentHand
            // 
            this.txtOpponentHand.Location = new System.Drawing.Point(1382, 775);
            this.txtOpponentHand.Margin = new System.Windows.Forms.Padding(6);
            this.txtOpponentHand.Multiline = true;
            this.txtOpponentHand.Name = "txtOpponentHand";
            this.txtOpponentHand.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtOpponentHand.Size = new System.Drawing.Size(325, 290);
            this.txtOpponentHand.TabIndex = 8;
            this.txtOpponentHand.WordWrap = false;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(354, 742);
            this.label6.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(119, 25);
            this.label6.TabIndex = 7;
            this.label6.Text = "Player Hand";
            // 
            // txtPlayerHand
            // 
            this.txtPlayerHand.Location = new System.Drawing.Point(354, 772);
            this.txtPlayerHand.Margin = new System.Windows.Forms.Padding(6);
            this.txtPlayerHand.Multiline = true;
            this.txtPlayerHand.Name = "txtPlayerHand";
            this.txtPlayerHand.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtPlayerHand.Size = new System.Drawing.Size(325, 290);
            this.txtPlayerHand.TabIndex = 6;
            this.txtPlayerHand.WordWrap = false;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(1377, 30);
            this.label5.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(146, 25);
            this.label5.TabIndex = 5;
            this.label5.Text = "Opponent Hero";
            // 
            // txtOpponentHero
            // 
            this.txtOpponentHero.Location = new System.Drawing.Point(1377, 59);
            this.txtOpponentHero.Margin = new System.Windows.Forms.Padding(6);
            this.txtOpponentHero.Multiline = true;
            this.txtOpponentHero.Name = "txtOpponentHero";
            this.txtOpponentHero.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtOpponentHero.Size = new System.Drawing.Size(325, 346);
            this.txtOpponentHero.TabIndex = 4;
            this.txtOpponentHero.WordWrap = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(343, 26);
            this.label4.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(114, 25);
            this.label4.TabIndex = 3;
            this.label4.Text = "Player Hero";
            // 
            // txtPlayerHero
            // 
            this.txtPlayerHero.Location = new System.Drawing.Point(354, 55);
            this.txtPlayerHero.Margin = new System.Windows.Forms.Padding(6);
            this.txtPlayerHero.Multiline = true;
            this.txtPlayerHero.Name = "txtPlayerHero";
            this.txtPlayerHero.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtPlayerHero.Size = new System.Drawing.Size(314, 346);
            this.txtPlayerHero.TabIndex = 2;
            this.txtPlayerHero.WordWrap = false;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(15, 26);
            this.label3.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(118, 25);
            this.label3.TabIndex = 1;
            this.label3.Text = "Game Entity";
            // 
            // txtGameEntity
            // 
            this.txtGameEntity.Location = new System.Drawing.Point(15, 55);
            this.txtGameEntity.Margin = new System.Windows.Forms.Padding(6);
            this.txtGameEntity.Multiline = true;
            this.txtGameEntity.Name = "txtGameEntity";
            this.txtGameEntity.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.txtGameEntity.Size = new System.Drawing.Size(325, 401);
            this.txtGameEntity.TabIndex = 0;
            this.txtGameEntity.WordWrap = false;
            // 
            // tabPageAI
            // 
            this.tabPageAI.Controls.Add(this.txtAIShell);
            this.tabPageAI.Controls.Add(this.label18);
            this.tabPageAI.Controls.Add(this.txtAIEngine);
            this.tabPageAI.Location = new System.Drawing.Point(4, 33);
            this.tabPageAI.Name = "tabPageAI";
            this.tabPageAI.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageAI.Size = new System.Drawing.Size(1737, 1421);
            this.tabPageAI.TabIndex = 3;
            this.tabPageAI.Text = "AI";
            this.tabPageAI.UseVisualStyleBackColor = true;
            this.tabPageAI.Click += new System.EventHandler(this.tabPage3_Click);
            // 
            // txtAIShell
            // 
            this.txtAIShell.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAIShell.Location = new System.Drawing.Point(165, 1389);
            this.txtAIShell.Multiline = true;
            this.txtAIShell.Name = "txtAIShell";
            this.txtAIShell.Size = new System.Drawing.Size(1566, 29);
            this.txtAIShell.TabIndex = 2;
            this.txtAIShell.TextChanged += new System.EventHandler(this.txtAIShell_TextChanged);
            this.txtAIShell.KeyDown += new System.Windows.Forms.KeyEventHandler(this.txtAIShell_KeyDown);
            this.txtAIShell.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.txtAIShell_KeyPress);
            // 
            // label18
            // 
            this.label18.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label18.AutoSize = true;
            this.label18.Location = new System.Drawing.Point(3, 1391);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(156, 25);
            this.label18.TabIndex = 1;
            this.label18.Text = "Interactive Shell:";
            // 
            // txtAIEngine
            // 
            this.txtAIEngine.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtAIEngine.Location = new System.Drawing.Point(3, 3);
            this.txtAIEngine.Multiline = true;
            this.txtAIEngine.Name = "txtAIEngine";
            this.txtAIEngine.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtAIEngine.Size = new System.Drawing.Size(1731, 1380);
            this.txtAIEngine.TabIndex = 0;
            // 
            // timerMainLoop
            // 
            this.timerMainLoop.Tick += new System.EventHandler(this.timerMainLoop_Tick);
            // 
            // frmMain
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(11F, 24F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1745, 1458);
            this.Controls.Add(this.tabControl1);
            this.Margin = new System.Windows.Forms.Padding(6);
            this.Name = "frmMain";
            this.Text = "Hearthstone AI";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.frmMain_FormClosing);
            this.Load += new System.EventHandler(this.frmMain_Load);
            this.tabControl1.ResumeLayout(false);
            this.tabPageSettings.ResumeLayout(false);
            this.tabPageSettings.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nudRootSampleCount)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudThreads)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nudSeconds)).EndInit();
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPageAI.ResumeLayout(false);
            this.tabPageAI.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPageSettings;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timerMainLoop;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.Button btnChangeHearthstoneInstallationPath;
        private System.Windows.Forms.TextBox txtHearthstoneInstallationPath;
        private System.Windows.Forms.Button btnStart;
        private System.Windows.Forms.ListBox listBoxProcessedLogs;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TextBox txtGameEntity;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox txtPlayerHero;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox txtOpponentHero;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox txtPlayerHand;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox txtOpponentHand;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtPlayerDeck;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox txtOpponentDeck;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox txtPlayerMinions;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.TextBox txtPlayerSecrets;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox txtOpponentSecrets;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.TextBox txtOpponentMinions;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.TextBox txtChoices;
        private System.Windows.Forms.TabPage tabPageAI;
        private System.Windows.Forms.TextBox txtAIEngine;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.NumericUpDown nudThreads;
        private System.Windows.Forms.NumericUpDown nudSeconds;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.NumericUpDown nudRootSampleCount;
        private System.Windows.Forms.TextBox txtAIShell;
        private System.Windows.Forms.Label label18;
    }
}

