namespace RealLauncher
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.FullScreen = new System.Windows.Forms.CheckBox();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.PlayButtonDefault = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.PlayButtonClick = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.PlayButtonDefault)).BeginInit();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PlayButtonClick)).BeginInit();
            this.SuspendLayout();
            // 
            // FullScreen
            // 
            this.FullScreen.AutoSize = true;
            this.FullScreen.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.FullScreen.Checked = true;
            this.FullScreen.CheckState = System.Windows.Forms.CheckState.Checked;
            this.FullScreen.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
            this.FullScreen.Location = new System.Drawing.Point(321, 625);
            this.FullScreen.Name = "FullScreen";
            this.FullScreen.Size = new System.Drawing.Size(76, 17);
            this.FullScreen.TabIndex = 1;
            this.FullScreen.Text = "FullScreen";
            this.FullScreen.UseVisualStyleBackColor = false;
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(130, 621);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(121, 21);
            this.comboBox1.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.label1.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
            this.label1.Location = new System.Drawing.Point(257, 626);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Resolution";
            // 
            // timer1
            // 
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // timer2
            // 
            this.timer2.Enabled = true;
            // 
            // PlayButtonDefault
            // 
            this.PlayButtonDefault.BackColor = System.Drawing.Color.Transparent;
            this.PlayButtonDefault.BackgroundImage = global::RealLauncher.Properties.Resources.play_default;
            this.PlayButtonDefault.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.PlayButtonDefault.Location = new System.Drawing.Point(4, -1);
            this.PlayButtonDefault.Name = "PlayButtonDefault";
            this.PlayButtonDefault.Size = new System.Drawing.Size(381, 94);
            this.PlayButtonDefault.TabIndex = 5;
            this.PlayButtonDefault.TabStop = false;
            this.PlayButtonDefault.Click += new System.EventHandler(this.button1_Click);
            this.PlayButtonDefault.MouseEnter += new System.EventHandler(this.PlayButtonDefault_MouseEnter);
            // 
            // panel1
            // 
            this.panel1.BackColor = System.Drawing.Color.Transparent;
            this.panel1.Controls.Add(this.PlayButtonClick);
            this.panel1.Controls.Add(this.PlayButtonDefault);
            this.panel1.Location = new System.Drawing.Point(63, 490);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(391, 109);
            this.panel1.TabIndex = 6;
            // 
            // PlayButtonClick
            // 
            this.PlayButtonClick.BackColor = System.Drawing.Color.Transparent;
            this.PlayButtonClick.BackgroundImage = global::RealLauncher.Properties.Resources.play_active;
            this.PlayButtonClick.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.PlayButtonClick.Location = new System.Drawing.Point(0, -1);
            this.PlayButtonClick.Name = "PlayButtonClick";
            this.PlayButtonClick.Size = new System.Drawing.Size(381, 94);
            this.PlayButtonClick.TabIndex = 6;
            this.PlayButtonClick.TabStop = false;
            this.PlayButtonClick.Click += new System.EventHandler(this.button1_Click);
            this.PlayButtonClick.MouseLeave += new System.EventHandler(this.PlayButtonDefault_MouseLeave);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ControlDark;
            this.BackgroundImage = global::RealLauncher.Properties.Resources.background;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.ClientSize = new System.Drawing.Size(519, 770);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comboBox1);
            this.Controls.Add(this.FullScreen);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.Text = "Hat N\' Slash";
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.PlayButtonDefault)).EndInit();
            this.panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.PlayButtonClick)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.CheckBox FullScreen;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Timer timer2;
        private System.Windows.Forms.PictureBox PlayButtonDefault;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.PictureBox PlayButtonClick;
    }
}

