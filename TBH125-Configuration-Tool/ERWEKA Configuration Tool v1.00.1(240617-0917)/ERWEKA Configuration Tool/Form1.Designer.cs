namespace ERWEKA_Configuration_Tool
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.Scan_button = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button1 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.h_checkBox = new System.Windows.Forms.CheckBox();
            this.d_checkBox = new System.Windows.Forms.CheckBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.b_radioButton = new System.Windows.Forms.RadioButton();
            this.s_radioButton = new System.Windows.Forms.RadioButton();
            this.SN_textBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.Hard_textBox = new System.Windows.Forms.TextBox();
            this.Brand_textBox = new System.Windows.Forms.TextBox();
            this.Id_textBox = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.Get_button = new System.Windows.Forms.Button();
            this.Save_button = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.label7 = new System.Windows.Forms.Label();
            this.Firmware_textBox = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(48, 34);
            this.comboBox1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(103, 28);
            this.comboBox1.TabIndex = 0;
            // 
            // Scan_button
            // 
            this.Scan_button.BackColor = System.Drawing.SystemColors.HotTrack;
            this.Scan_button.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Scan_button.Location = new System.Drawing.Point(199, 32);
            this.Scan_button.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Scan_button.Name = "Scan_button";
            this.Scan_button.Size = new System.Drawing.Size(75, 31);
            this.Scan_button.TabIndex = 1;
            this.Scan_button.Text = "Scan";
            this.Scan_button.UseVisualStyleBackColor = false;
            this.Scan_button.Click += new System.EventHandler(this.Scan_button_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button1);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.comboBox1);
            this.groupBox1.Controls.Add(this.Scan_button);
            this.groupBox1.Location = new System.Drawing.Point(22, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(467, 85);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            // 
            // button1
            // 
            this.button1.BackColor = System.Drawing.Color.Silver;
            this.button1.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(338, 32);
            this.button1.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 31);
            this.button1.TabIndex = 5;
            this.button1.Text = "Open";
            this.button1.UseVisualStyleBackColor = false;
            this.button1.Visible = false;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(9, 35);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(40, 24);
            this.label1.TabIndex = 4;
            this.label1.Text = "Port";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.h_checkBox);
            this.groupBox2.Controls.Add(this.d_checkBox);
            this.groupBox2.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox2.Location = new System.Drawing.Point(22, 251);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(187, 96);
            this.groupBox2.TabIndex = 5;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Configuration";
            // 
            // h_checkBox
            // 
            this.h_checkBox.AutoSize = true;
            this.h_checkBox.Location = new System.Drawing.Point(13, 55);
            this.h_checkBox.Name = "h_checkBox";
            this.h_checkBox.Size = new System.Drawing.Size(104, 28);
            this.h_checkBox.TabIndex = 5;
            this.h_checkBox.Text = "Thickness";
            this.h_checkBox.UseVisualStyleBackColor = true;
            // 
            // d_checkBox
            // 
            this.d_checkBox.AutoSize = true;
            this.d_checkBox.Location = new System.Drawing.Point(13, 25);
            this.d_checkBox.Name = "d_checkBox";
            this.d_checkBox.Size = new System.Drawing.Size(99, 28);
            this.d_checkBox.TabIndex = 4;
            this.d_checkBox.Text = "Diameter";
            this.d_checkBox.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.b_radioButton);
            this.groupBox4.Controls.Add(this.s_radioButton);
            this.groupBox4.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.groupBox4.Location = new System.Drawing.Point(302, 251);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(187, 96);
            this.groupBox4.TabIndex = 3;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Measurement Range";
            // 
            // b_radioButton
            // 
            this.b_radioButton.AutoSize = true;
            this.b_radioButton.Location = new System.Drawing.Point(10, 60);
            this.b_radioButton.Name = "b_radioButton";
            this.b_radioButton.Size = new System.Drawing.Size(70, 28);
            this.b_radioButton.TabIndex = 1;
            this.b_radioButton.TabStop = true;
            this.b_radioButton.Text = "500N";
            this.b_radioButton.UseVisualStyleBackColor = true;
            // 
            // s_radioButton
            // 
            this.s_radioButton.AutoSize = true;
            this.s_radioButton.Location = new System.Drawing.Point(9, 25);
            this.s_radioButton.Name = "s_radioButton";
            this.s_radioButton.Size = new System.Drawing.Size(70, 28);
            this.s_radioButton.TabIndex = 0;
            this.s_radioButton.TabStop = true;
            this.s_radioButton.Text = "300N";
            this.s_radioButton.UseVisualStyleBackColor = true;
            // 
            // SN_textBox
            // 
            this.SN_textBox.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.SN_textBox.Location = new System.Drawing.Point(193, 199);
            this.SN_textBox.MaxLength = 6;
            this.SN_textBox.Multiline = true;
            this.SN_textBox.Name = "SN_textBox";
            this.SN_textBox.Size = new System.Drawing.Size(124, 29);
            this.SN_textBox.TabIndex = 14;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.label6.Font = new System.Drawing.Font("Arial Narrow", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(189, 176);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(128, 22);
            this.label6.TabIndex = 14;
            this.label6.Text = "Serial Number      ";
            // 
            // Hard_textBox
            // 
            this.Hard_textBox.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Hard_textBox.Location = new System.Drawing.Point(22, 131);
            this.Hard_textBox.Multiline = true;
            this.Hard_textBox.Name = "Hard_textBox";
            this.Hard_textBox.Size = new System.Drawing.Size(136, 29);
            this.Hard_textBox.TabIndex = 6;
            // 
            // Brand_textBox
            // 
            this.Brand_textBox.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Brand_textBox.Location = new System.Drawing.Point(360, 132);
            this.Brand_textBox.Multiline = true;
            this.Brand_textBox.Name = "Brand_textBox";
            this.Brand_textBox.Size = new System.Drawing.Size(129, 28);
            this.Brand_textBox.TabIndex = 8;
            // 
            // Id_textBox
            // 
            this.Id_textBox.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Id_textBox.Location = new System.Drawing.Point(22, 199);
            this.Id_textBox.Multiline = true;
            this.Id_textBox.Name = "Id_textBox";
            this.Id_textBox.Size = new System.Drawing.Size(136, 29);
            this.Id_textBox.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.label2.Font = new System.Drawing.Font("Arial Narrow", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(22, 107);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(126, 22);
            this.label2.TabIndex = 10;
            this.label2.Text = "Hardware Version";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.label4.Font = new System.Drawing.Font("Arial Narrow", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(356, 106);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(49, 22);
            this.label4.TabIndex = 12;
            this.label4.Text = "Brand";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.label5.Font = new System.Drawing.Font("Arial Narrow", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(23, 176);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(81, 22);
            this.label5.TabIndex = 13;
            this.label5.Text = "ID Number";
            // 
            // Get_button
            // 
            this.Get_button.BackColor = System.Drawing.Color.Silver;
            this.Get_button.Enabled = false;
            this.Get_button.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Get_button.Location = new System.Drawing.Point(35, 365);
            this.Get_button.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Get_button.Name = "Get_button";
            this.Get_button.Size = new System.Drawing.Size(75, 31);
            this.Get_button.TabIndex = 14;
            this.Get_button.Text = "Get";
            this.Get_button.UseVisualStyleBackColor = false;
            this.Get_button.Click += new System.EventHandler(this.Get_button_Click);
            // 
            // Save_button
            // 
            this.Save_button.BackColor = System.Drawing.Color.Silver;
            this.Save_button.Enabled = false;
            this.Save_button.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Save_button.Location = new System.Drawing.Point(386, 365);
            this.Save_button.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.Save_button.Name = "Save_button";
            this.Save_button.Size = new System.Drawing.Size(75, 31);
            this.Save_button.TabIndex = 15;
            this.Save_button.Text = "Save";
            this.Save_button.UseVisualStyleBackColor = false;
            this.Save_button.Click += new System.EventHandler(this.Save_button_Click);
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.label7.Font = new System.Drawing.Font("Arial Narrow", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(189, 106);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(124, 22);
            this.label7.TabIndex = 17;
            this.label7.Text = "Firmware Version";
            // 
            // Firmware_textBox
            // 
            this.Firmware_textBox.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Firmware_textBox.Location = new System.Drawing.Point(193, 131);
            this.Firmware_textBox.Multiline = true;
            this.Firmware_textBox.Name = "Firmware_textBox";
            this.Firmware_textBox.Size = new System.Drawing.Size(124, 29);
            this.Firmware_textBox.TabIndex = 16;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.LightSteelBlue;
            this.ClientSize = new System.Drawing.Size(518, 418);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.Firmware_textBox);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.Save_button);
            this.Controls.Add(this.SN_textBox);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.Get_button);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.Id_textBox);
            this.Controls.Add(this.Brand_textBox);
            this.Controls.Add(this.Hard_textBox);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Font = new System.Drawing.Font("Arial Narrow", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(3, 4, 3, 4);
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "ERWEKA Configuration Tool v1.00.1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.Button Scan_button;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox Hard_textBox;
        private System.Windows.Forms.TextBox Brand_textBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckBox h_checkBox;
        private System.Windows.Forms.CheckBox d_checkBox;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.RadioButton b_radioButton;
        private System.Windows.Forms.RadioButton s_radioButton;
        private System.Windows.Forms.TextBox SN_textBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button Get_button;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox Firmware_textBox;
        private System.Windows.Forms.TextBox Id_textBox;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button Save_button;
    }
}

