namespace XplaneTesting
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
            this.IP_groupBox = new System.Windows.Forms.GroupBox();
            this.PublicIP_button = new System.Windows.Forms.Button();
            this.ClientIP_textBox = new System.Windows.Forms.TextBox();
            this.Port_textBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.TextBoxClear_button = new System.Windows.Forms.Button();
            this.debug_textBox = new System.Windows.Forms.TextBox();
            this.Debug1_button = new System.Windows.Forms.Button();
            this.Housekeeping_timer = new System.Windows.Forms.Timer(this.components);
            this.IP_groupBox.SuspendLayout();
            this.SuspendLayout();
            // 
            // IP_groupBox
            // 
            this.IP_groupBox.Controls.Add(this.PublicIP_button);
            this.IP_groupBox.Controls.Add(this.ClientIP_textBox);
            this.IP_groupBox.Controls.Add(this.Port_textBox);
            this.IP_groupBox.Controls.Add(this.label1);
            this.IP_groupBox.Controls.Add(this.label9);
            this.IP_groupBox.Location = new System.Drawing.Point(12, 12);
            this.IP_groupBox.Name = "IP_groupBox";
            this.IP_groupBox.Size = new System.Drawing.Size(281, 79);
            this.IP_groupBox.TabIndex = 53;
            this.IP_groupBox.TabStop = false;
            this.IP_groupBox.Text = "IP stuff";
            // 
            // PublicIP_button
            // 
            this.PublicIP_button.Location = new System.Drawing.Point(164, 47);
            this.PublicIP_button.Name = "PublicIP_button";
            this.PublicIP_button.Size = new System.Drawing.Size(95, 23);
            this.PublicIP_button.TabIndex = 53;
            this.PublicIP_button.Text = "Get My Public IP";
            this.PublicIP_button.UseVisualStyleBackColor = true;
            this.PublicIP_button.Click += new System.EventHandler(this.PublicIP_button_Click);
            // 
            // ClientIP_textBox
            // 
            this.ClientIP_textBox.Location = new System.Drawing.Point(57, 19);
            this.ClientIP_textBox.Multiline = true;
            this.ClientIP_textBox.Name = "ClientIP_textBox";
            this.ClientIP_textBox.Size = new System.Drawing.Size(202, 24);
            this.ClientIP_textBox.TabIndex = 41;
            this.ClientIP_textBox.Text = "192.168.11.200";
            // 
            // Port_textBox
            // 
            this.Port_textBox.Location = new System.Drawing.Point(57, 49);
            this.Port_textBox.Multiline = true;
            this.Port_textBox.Name = "Port_textBox";
            this.Port_textBox.Size = new System.Drawing.Size(59, 24);
            this.Port_textBox.TabIndex = 34;
            this.Port_textBox.Text = "3003";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 52);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(29, 13);
            this.label1.TabIndex = 35;
            this.label1.Text = "Port:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(9, 22);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(49, 13);
            this.label9.TabIndex = 42;
            this.label9.Text = "Client IP:";
            // 
            // TextBoxClear_button
            // 
            this.TextBoxClear_button.Location = new System.Drawing.Point(585, 264);
            this.TextBoxClear_button.Name = "TextBoxClear_button";
            this.TextBoxClear_button.Size = new System.Drawing.Size(87, 32);
            this.TextBoxClear_button.TabIndex = 55;
            this.TextBoxClear_button.Text = "Clear";
            this.TextBoxClear_button.UseVisualStyleBackColor = true;
            // 
            // debug_textBox
            // 
            this.debug_textBox.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.debug_textBox.Location = new System.Drawing.Point(299, 12);
            this.debug_textBox.Multiline = true;
            this.debug_textBox.Name = "debug_textBox";
            this.debug_textBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.debug_textBox.Size = new System.Drawing.Size(373, 246);
            this.debug_textBox.TabIndex = 54;
            // 
            // Debug1_button
            // 
            this.Debug1_button.Location = new System.Drawing.Point(41, 183);
            this.Debug1_button.Name = "Debug1_button";
            this.Debug1_button.Size = new System.Drawing.Size(87, 32);
            this.Debug1_button.TabIndex = 56;
            this.Debug1_button.Text = "Debug1";
            this.Debug1_button.UseVisualStyleBackColor = true;
            this.Debug1_button.Click += new System.EventHandler(this.Debug1_button_Click);
            // 
            // Housekeeping_timer
            // 
            this.Housekeeping_timer.Enabled = true;
            this.Housekeeping_timer.Tick += new System.EventHandler(this.Housekeeping_timer_Tick);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(684, 298);
            this.Controls.Add(this.Debug1_button);
            this.Controls.Add(this.TextBoxClear_button);
            this.Controls.Add(this.debug_textBox);
            this.Controls.Add(this.IP_groupBox);
            this.Name = "MainForm";
            this.Text = "XplaneTesting";
            this.Load += new System.EventHandler(this.MainForm_Load);
            this.IP_groupBox.ResumeLayout(false);
            this.IP_groupBox.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox IP_groupBox;
        private System.Windows.Forms.Button PublicIP_button;
        private System.Windows.Forms.TextBox ClientIP_textBox;
        private System.Windows.Forms.TextBox Port_textBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Button TextBoxClear_button;
        private System.Windows.Forms.TextBox debug_textBox;
        private System.Windows.Forms.Button Debug1_button;
        private System.Windows.Forms.Timer Housekeeping_timer;
    }
}

