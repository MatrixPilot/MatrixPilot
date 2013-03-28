namespace GPSspoof
{
    partial class MainForm
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
            this.HouseKeeping_1sec_timer = new System.Windows.Forms.Timer(this.components);
            this.IsConnected_radioButton = new System.Windows.Forms.RadioButton();
            this.ClientDisconnect_button = new System.Windows.Forms.Button();
            this.ClientIP_textBox = new System.Windows.Forms.TextBox();
            this.Port_textBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.Connect_checkBox = new System.Windows.Forms.CheckBox();
            this.debug_textBox = new System.Windows.Forms.TextBox();
            this.TextBoxClear_button = new System.Windows.Forms.Button();
            this.HouseKeeping_100ms_timer = new System.Windows.Forms.Timer(this.components);
            this.label1 = new System.Windows.Forms.Label();
            this.Lat_textBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.Long_textBox = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.Alt_textBox = new System.Windows.Forms.TextBox();
            this.SpoofMode_comboBox = new System.Windows.Forms.ComboBox();
            this.SendDataOnce_button = new System.Windows.Forms.Button();
            this.SendDataRepeat_checkBox = new System.Windows.Forms.CheckBox();
            this.SendDataInterval_textBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.SendData_timer = new System.Windows.Forms.Timer(this.components);
            this.CopyLatLong_button = new System.Windows.Forms.Button();
            this.AltReset_textBox = new System.Windows.Forms.TextBox();
            this.LongReset_textBox = new System.Windows.Forms.TextBox();
            this.LatReset_textBox = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.ResetLatLong_button = new System.Windows.Forms.Button();
            this.AltOffset_textBox = new System.Windows.Forms.TextBox();
            this.LongOffset_textBox = new System.Windows.Forms.TextBox();
            this.label12 = new System.Windows.Forms.Label();
            this.LatOffset_textBox = new System.Windows.Forms.TextBox();
            this.LatInc_textBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.LongInc_textBox = new System.Windows.Forms.TextBox();
            this.AltInc_textBox = new System.Windows.Forms.TextBox();
            this.label13 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // HouseKeeping_1sec_timer
            // 
            this.HouseKeeping_1sec_timer.Enabled = true;
            this.HouseKeeping_1sec_timer.Interval = 1000;
            this.HouseKeeping_1sec_timer.Tick += new System.EventHandler(this.HouseKeeping_1sec_timer_Tick);
            // 
            // IsConnected_radioButton
            // 
            this.IsConnected_radioButton.AutoSize = true;
            this.IsConnected_radioButton.Location = new System.Drawing.Point(49, 335);
            this.IsConnected_radioButton.Name = "IsConnected_radioButton";
            this.IsConnected_radioButton.Size = new System.Drawing.Size(77, 17);
            this.IsConnected_radioButton.TabIndex = 4;
            this.IsConnected_radioButton.Text = "Connected";
            this.IsConnected_radioButton.UseVisualStyleBackColor = true;
            this.IsConnected_radioButton.CheckedChanged += new System.EventHandler(this.IsConnected_radioButton_CheckedChanged);
            // 
            // ClientDisconnect_button
            // 
            this.ClientDisconnect_button.Location = new System.Drawing.Point(194, 305);
            this.ClientDisconnect_button.Name = "ClientDisconnect_button";
            this.ClientDisconnect_button.Size = new System.Drawing.Size(75, 27);
            this.ClientDisconnect_button.TabIndex = 44;
            this.ClientDisconnect_button.Text = "Disconnect";
            this.ClientDisconnect_button.UseVisualStyleBackColor = true;
            this.ClientDisconnect_button.Click += new System.EventHandler(this.ClientDisconnect_button_Click);
            // 
            // ClientIP_textBox
            // 
            this.ClientIP_textBox.Location = new System.Drawing.Point(67, 275);
            this.ClientIP_textBox.Multiline = true;
            this.ClientIP_textBox.Name = "ClientIP_textBox";
            this.ClientIP_textBox.Size = new System.Drawing.Size(202, 24);
            this.ClientIP_textBox.TabIndex = 47;
            this.ClientIP_textBox.Text = "uav";
            this.ClientIP_textBox.TextChanged += new System.EventHandler(this.ClientIP_textBox_TextChanged);
            // 
            // Port_textBox
            // 
            this.Port_textBox.Location = new System.Drawing.Point(67, 305);
            this.Port_textBox.Multiline = true;
            this.Port_textBox.Name = "Port_textBox";
            this.Port_textBox.Size = new System.Drawing.Size(59, 24);
            this.Port_textBox.TabIndex = 45;
            this.Port_textBox.Text = "3007";
            this.Port_textBox.TextChanged += new System.EventHandler(this.Port_textBox_TextChanged);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(32, 308);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(29, 13);
            this.label2.TabIndex = 46;
            this.label2.Text = "Port:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(19, 278);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(46, 13);
            this.label9.TabIndex = 48;
            this.label9.Text = "UDB IP:";
            // 
            // Connect_checkBox
            // 
            this.Connect_checkBox.AutoSize = true;
            this.Connect_checkBox.Location = new System.Drawing.Point(190, 341);
            this.Connect_checkBox.Name = "Connect_checkBox";
            this.Connect_checkBox.Size = new System.Drawing.Size(66, 17);
            this.Connect_checkBox.TabIndex = 49;
            this.Connect_checkBox.Text = "Connect";
            this.Connect_checkBox.UseVisualStyleBackColor = true;
            this.Connect_checkBox.CheckedChanged += new System.EventHandler(this.Connect_checkBox_CheckedChanged);
            // 
            // debug_textBox
            // 
            this.debug_textBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.debug_textBox.Location = new System.Drawing.Point(445, 12);
            this.debug_textBox.Multiline = true;
            this.debug_textBox.Name = "debug_textBox";
            this.debug_textBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.debug_textBox.Size = new System.Drawing.Size(307, 320);
            this.debug_textBox.TabIndex = 50;
            // 
            // TextBoxClear_button
            // 
            this.TextBoxClear_button.Location = new System.Drawing.Point(665, 335);
            this.TextBoxClear_button.Name = "TextBoxClear_button";
            this.TextBoxClear_button.Size = new System.Drawing.Size(87, 32);
            this.TextBoxClear_button.TabIndex = 51;
            this.TextBoxClear_button.Text = "Clear";
            this.TextBoxClear_button.UseVisualStyleBackColor = true;
            this.TextBoxClear_button.Click += new System.EventHandler(this.TextBoxClear_button_Click);
            // 
            // HouseKeeping_100ms_timer
            // 
            this.HouseKeeping_100ms_timer.Enabled = true;
            this.HouseKeeping_100ms_timer.Tick += new System.EventHandler(this.HouseKeeping_100ms_timer_Tick);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(374, 335);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 13);
            this.label1.TabIndex = 52;
            this.label1.Text = "Debug";
            // 
            // Lat_textBox
            // 
            this.Lat_textBox.Location = new System.Drawing.Point(146, 44);
            this.Lat_textBox.Multiline = true;
            this.Lat_textBox.Name = "Lat_textBox";
            this.Lat_textBox.Size = new System.Drawing.Size(90, 21);
            this.Lat_textBox.TabIndex = 53;
            this.Lat_textBox.Text = "0";
            this.Lat_textBox.TextChanged += new System.EventHandler(this.Lat_textBox_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(120, 47);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(22, 13);
            this.label3.TabIndex = 54;
            this.label3.Text = "Lat";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(114, 74);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 56;
            this.label4.Text = "Long";
            // 
            // Long_textBox
            // 
            this.Long_textBox.Location = new System.Drawing.Point(146, 71);
            this.Long_textBox.Multiline = true;
            this.Long_textBox.Name = "Long_textBox";
            this.Long_textBox.Size = new System.Drawing.Size(90, 21);
            this.Long_textBox.TabIndex = 55;
            this.Long_textBox.Text = "0";
            this.Long_textBox.TextChanged += new System.EventHandler(this.Long_textBox_TextChanged);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(120, 101);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(19, 13);
            this.label5.TabIndex = 58;
            this.label5.Text = "Alt";
            // 
            // Alt_textBox
            // 
            this.Alt_textBox.Location = new System.Drawing.Point(146, 98);
            this.Alt_textBox.Multiline = true;
            this.Alt_textBox.Name = "Alt_textBox";
            this.Alt_textBox.Size = new System.Drawing.Size(90, 21);
            this.Alt_textBox.TabIndex = 57;
            this.Alt_textBox.Text = "0";
            this.Alt_textBox.TextChanged += new System.EventHandler(this.Alt_textBox_TextChanged);
            // 
            // SpoofMode_comboBox
            // 
            this.SpoofMode_comboBox.FormattingEnabled = true;
            this.SpoofMode_comboBox.Items.AddRange(new object[] {
            "Disabled",
            "Override",
            "Offset"});
            this.SpoofMode_comboBox.Location = new System.Drawing.Point(49, 195);
            this.SpoofMode_comboBox.Name = "SpoofMode_comboBox";
            this.SpoofMode_comboBox.Size = new System.Drawing.Size(65, 21);
            this.SpoofMode_comboBox.TabIndex = 59;
            this.SpoofMode_comboBox.SelectedIndexChanged += new System.EventHandler(this.SpoofMode_comboBox_SelectedIndexChanged);
            // 
            // SendDataOnce_button
            // 
            this.SendDataOnce_button.Location = new System.Drawing.Point(123, 191);
            this.SendDataOnce_button.Name = "SendDataOnce_button";
            this.SendDataOnce_button.Size = new System.Drawing.Size(75, 27);
            this.SendDataOnce_button.TabIndex = 60;
            this.SendDataOnce_button.Text = "Send Once";
            this.SendDataOnce_button.UseVisualStyleBackColor = true;
            this.SendDataOnce_button.Click += new System.EventHandler(this.SendDataOnce_button_Click);
            // 
            // SendDataRepeat_checkBox
            // 
            this.SendDataRepeat_checkBox.AutoSize = true;
            this.SendDataRepeat_checkBox.Location = new System.Drawing.Point(35, 168);
            this.SendDataRepeat_checkBox.Name = "SendDataRepeat_checkBox";
            this.SendDataRepeat_checkBox.Size = new System.Drawing.Size(134, 17);
            this.SendDataRepeat_checkBox.TabIndex = 61;
            this.SendDataRepeat_checkBox.Text = "Send Data Repeatedly";
            this.SendDataRepeat_checkBox.UseVisualStyleBackColor = true;
            this.SendDataRepeat_checkBox.CheckedChanged += new System.EventHandler(this.SendDataRepeat_checkBox_CheckedChanged);
            // 
            // SendDataInterval_textBox
            // 
            this.SendDataInterval_textBox.Location = new System.Drawing.Point(314, 158);
            this.SendDataInterval_textBox.Multiline = true;
            this.SendDataInterval_textBox.Name = "SendDataInterval_textBox";
            this.SendDataInterval_textBox.Size = new System.Drawing.Size(45, 20);
            this.SendDataInterval_textBox.TabIndex = 62;
            this.SendDataInterval_textBox.Text = "1000";
            this.SendDataInterval_textBox.TextChanged += new System.EventHandler(this.SendDataInterval_textBox_TextChanged);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(213, 161);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(95, 13);
            this.label6.TabIndex = 63;
            this.label6.Text = "Increment Interval:";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(365, 161);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(20, 13);
            this.label7.TabIndex = 64;
            this.label7.Text = "ms";
            // 
            // SendData_timer
            // 
            this.SendData_timer.Interval = 5000;
            this.SendData_timer.Tick += new System.EventHandler(this.SendData_timer_Tick);
            // 
            // CopyLatLong_button
            // 
            this.CopyLatLong_button.Location = new System.Drawing.Point(82, 6);
            this.CopyLatLong_button.Name = "CopyLatLong_button";
            this.CopyLatLong_button.Size = new System.Drawing.Size(69, 22);
            this.CopyLatLong_button.TabIndex = 69;
            this.CopyLatLong_button.Text = "Copy ->";
            this.CopyLatLong_button.UseVisualStyleBackColor = true;
            this.CopyLatLong_button.Visible = false;
            this.CopyLatLong_button.Click += new System.EventHandler(this.CopyLatLong_button_Click);
            // 
            // AltReset_textBox
            // 
            this.AltReset_textBox.Location = new System.Drawing.Point(18, 98);
            this.AltReset_textBox.Multiline = true;
            this.AltReset_textBox.Name = "AltReset_textBox";
            this.AltReset_textBox.Size = new System.Drawing.Size(96, 21);
            this.AltReset_textBox.TabIndex = 72;
            this.AltReset_textBox.Text = "0";
            this.AltReset_textBox.Visible = false;
            this.AltReset_textBox.TextChanged += new System.EventHandler(this.AltReset_textBox_TextChanged);
            // 
            // LongReset_textBox
            // 
            this.LongReset_textBox.Location = new System.Drawing.Point(18, 71);
            this.LongReset_textBox.Multiline = true;
            this.LongReset_textBox.Name = "LongReset_textBox";
            this.LongReset_textBox.Size = new System.Drawing.Size(96, 21);
            this.LongReset_textBox.TabIndex = 71;
            this.LongReset_textBox.Text = "0";
            this.LongReset_textBox.Visible = false;
            this.LongReset_textBox.TextChanged += new System.EventHandler(this.LongReset_textBox_TextChanged);
            // 
            // LatReset_textBox
            // 
            this.LatReset_textBox.Location = new System.Drawing.Point(18, 44);
            this.LatReset_textBox.Multiline = true;
            this.LatReset_textBox.Name = "LatReset_textBox";
            this.LatReset_textBox.Size = new System.Drawing.Size(96, 21);
            this.LatReset_textBox.TabIndex = 70;
            this.LatReset_textBox.Text = "0";
            this.LatReset_textBox.Visible = false;
            this.LatReset_textBox.TextChanged += new System.EventHandler(this.LatReset_textBox_TextChanged);
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(156, 28);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(72, 13);
            this.label10.TabIndex = 73;
            this.label10.Text = "Override GPS";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(46, 28);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(41, 13);
            this.label11.TabIndex = 74;
            this.label11.Text = "Default";
            this.label11.Visible = false;
            // 
            // ResetLatLong_button
            // 
            this.ResetLatLong_button.Location = new System.Drawing.Point(18, 20);
            this.ResetLatLong_button.Name = "ResetLatLong_button";
            this.ResetLatLong_button.Size = new System.Drawing.Size(18, 21);
            this.ResetLatLong_button.TabIndex = 75;
            this.ResetLatLong_button.Text = "X";
            this.ResetLatLong_button.UseVisualStyleBackColor = true;
            this.ResetLatLong_button.Visible = false;
            this.ResetLatLong_button.Click += new System.EventHandler(this.ResetLatLong_button_Click);
            // 
            // AltOffset_textBox
            // 
            this.AltOffset_textBox.Location = new System.Drawing.Point(242, 98);
            this.AltOffset_textBox.Multiline = true;
            this.AltOffset_textBox.Name = "AltOffset_textBox";
            this.AltOffset_textBox.Size = new System.Drawing.Size(77, 20);
            this.AltOffset_textBox.TabIndex = 79;
            this.AltOffset_textBox.Text = "0";
            this.AltOffset_textBox.TextChanged += new System.EventHandler(this.AltOffset_textBox_TextChanged);
            // 
            // LongOffset_textBox
            // 
            this.LongOffset_textBox.Location = new System.Drawing.Point(242, 72);
            this.LongOffset_textBox.Multiline = true;
            this.LongOffset_textBox.Name = "LongOffset_textBox";
            this.LongOffset_textBox.Size = new System.Drawing.Size(77, 20);
            this.LongOffset_textBox.TabIndex = 78;
            this.LongOffset_textBox.Text = "0";
            this.LongOffset_textBox.TextChanged += new System.EventHandler(this.LongOffset_textBox_TextChanged);
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(273, 28);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(35, 13);
            this.label12.TabIndex = 77;
            this.label12.Text = "Offset";
            // 
            // LatOffset_textBox
            // 
            this.LatOffset_textBox.Location = new System.Drawing.Point(242, 44);
            this.LatOffset_textBox.Multiline = true;
            this.LatOffset_textBox.Name = "LatOffset_textBox";
            this.LatOffset_textBox.Size = new System.Drawing.Size(77, 20);
            this.LatOffset_textBox.TabIndex = 76;
            this.LatOffset_textBox.Text = "0";
            this.LatOffset_textBox.TextChanged += new System.EventHandler(this.LatOffset_textBox_TextChanged);
            // 
            // LatInc_textBox
            // 
            this.LatInc_textBox.Location = new System.Drawing.Point(336, 44);
            this.LatInc_textBox.Multiline = true;
            this.LatInc_textBox.Name = "LatInc_textBox";
            this.LatInc_textBox.Size = new System.Drawing.Size(77, 20);
            this.LatInc_textBox.TabIndex = 65;
            this.LatInc_textBox.Text = "0";
            this.LatInc_textBox.TextChanged += new System.EventHandler(this.LatInc_textBox_TextChanged);
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(347, 28);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(54, 13);
            this.label8.TabIndex = 66;
            this.label8.Text = "Increment";
            // 
            // LongInc_textBox
            // 
            this.LongInc_textBox.Location = new System.Drawing.Point(336, 71);
            this.LongInc_textBox.Multiline = true;
            this.LongInc_textBox.Name = "LongInc_textBox";
            this.LongInc_textBox.Size = new System.Drawing.Size(77, 20);
            this.LongInc_textBox.TabIndex = 67;
            this.LongInc_textBox.Text = "0";
            this.LongInc_textBox.TextChanged += new System.EventHandler(this.LongInc_textBox_TextChanged);
            // 
            // AltInc_textBox
            // 
            this.AltInc_textBox.Location = new System.Drawing.Point(336, 98);
            this.AltInc_textBox.Multiline = true;
            this.AltInc_textBox.Name = "AltInc_textBox";
            this.AltInc_textBox.Size = new System.Drawing.Size(77, 20);
            this.AltInc_textBox.TabIndex = 68;
            this.AltInc_textBox.Text = "0";
            this.AltInc_textBox.TextChanged += new System.EventHandler(this.AltInc_textBox_TextChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(15, 198);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(37, 13);
            this.label13.TabIndex = 80;
            this.label13.Text = "Mode:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(774, 371);
            this.Controls.Add(this.label13);
            this.Controls.Add(this.AltOffset_textBox);
            this.Controls.Add(this.LongOffset_textBox);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.LatOffset_textBox);
            this.Controls.Add(this.ResetLatLong_button);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.AltReset_textBox);
            this.Controls.Add(this.LongReset_textBox);
            this.Controls.Add(this.LatReset_textBox);
            this.Controls.Add(this.CopyLatLong_button);
            this.Controls.Add(this.AltInc_textBox);
            this.Controls.Add(this.LongInc_textBox);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.LatInc_textBox);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.SendDataInterval_textBox);
            this.Controls.Add(this.SendDataRepeat_checkBox);
            this.Controls.Add(this.SendDataOnce_button);
            this.Controls.Add(this.SpoofMode_comboBox);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.Alt_textBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.Long_textBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.Lat_textBox);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.TextBoxClear_button);
            this.Controls.Add(this.debug_textBox);
            this.Controls.Add(this.Connect_checkBox);
            this.Controls.Add(this.ClientIP_textBox);
            this.Controls.Add(this.Port_textBox);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.ClientDisconnect_button);
            this.Controls.Add(this.IsConnected_radioButton);
            this.Name = "MainForm";
            this.Text = "UAV Dev Board - GPS spoofing tool - by Tom Pittenger";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainForm_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Timer HouseKeeping_1sec_timer;
        private System.Windows.Forms.RadioButton IsConnected_radioButton;
        private System.Windows.Forms.Button ClientDisconnect_button;
        private System.Windows.Forms.TextBox ClientIP_textBox;
        private System.Windows.Forms.TextBox Port_textBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label9;
        public System.Windows.Forms.CheckBox Connect_checkBox;
        private System.Windows.Forms.TextBox debug_textBox;
        private System.Windows.Forms.Button TextBoxClear_button;
        private System.Windows.Forms.Timer HouseKeeping_100ms_timer;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox Lat_textBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox Long_textBox;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox Alt_textBox;
        private System.Windows.Forms.ComboBox SpoofMode_comboBox;
        private System.Windows.Forms.Button SendDataOnce_button;
        private System.Windows.Forms.CheckBox SendDataRepeat_checkBox;
        private System.Windows.Forms.TextBox SendDataInterval_textBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Timer SendData_timer;
        private System.Windows.Forms.Button CopyLatLong_button;
        private System.Windows.Forms.TextBox AltReset_textBox;
        private System.Windows.Forms.TextBox LongReset_textBox;
        private System.Windows.Forms.TextBox LatReset_textBox;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Button ResetLatLong_button;
        private System.Windows.Forms.TextBox AltOffset_textBox;
        private System.Windows.Forms.TextBox LongOffset_textBox;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox LatOffset_textBox;
        private System.Windows.Forms.TextBox LatInc_textBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox LongInc_textBox;
        private System.Windows.Forms.TextBox AltInc_textBox;
        private System.Windows.Forms.Label label13;
    }
}

