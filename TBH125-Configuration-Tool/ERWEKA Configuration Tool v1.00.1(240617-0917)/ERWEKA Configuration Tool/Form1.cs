using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace ERWEKA_Configuration_Tool
{
    public partial class Form1 : Form
    {
        string str1="";
        string Updata_str = "";
        public Form1()
        {
            InitializeComponent();
        }

        private void Search_AddSerialToComboBox(SerialPort MyPort, ComboBox MyBox)//串口扫描函数
        {
            //将可用端口号添加到ComboBox
            string[] MyString = new string[20]; //存储20个
            string Buffer; //缓存
            string[] opened_port = new string[20]; //记录可以打开的,其实只是为了记住第一个可以打开的,供后面初始化
            int count_opendPort = 0;
            MyBox.Items.Clear(); //清空ComboBox内容
            for (int i = 1; i < 20; i++) //循环
            {
                try //核心原理是依靠try和catch完成遍历
                {
                    Buffer = "COM" + i.ToString();
                    MyPort.PortName = Buffer;
                    MyPort.Open(); //如果失败，后面的代码不会执行
                    opened_port[count_opendPort++] = Buffer;
                    MyBox.Items.Add(Buffer); //打开成功，添加至下俩列表
                    //button1.BackColor = Color.Blue;
                    button1.Visible = true;
                    MyPort.Close(); //关闭
                }
                catch
                {

                }
            }
            MyBox.Text = opened_port[0]; //初始化第一个打开的,若都没打开则默认为空
        }

        private void Scan_button_Click(object sender, EventArgs e)
        {
            Search_AddSerialToComboBox(serialPort1, comboBox1);
        }

        private void Get_button_Click(object sender, EventArgs e)
        {

            Get_button.Enabled = false;
            Save_button.Enabled = true;

            serialPort1.Write("HQ\r\n");             //发送HQ给单片机
            Delay(500);                              //适当延时避免处理错误
            try
            {
                Hard_textBox.Text = str1.Substring(86, 7);
                //Date_textBox.Text = str1.Substring(14, 8);
                Firmware_textBox.Text = str1.Substring(1, 4);

                if (str1.Substring(23, 1) == "E")          //公司名称判断
                    Brand_textBox.Text = "ERWEKA";
                else
                    Brand_textBox.Text = "NULL";

                Id_textBox.Text = str1.Substring(93, 14);

                if (str1.Substring(45, 1) == "1")          //直径判断
                    d_checkBox.Checked = true;
                else
                    d_checkBox.Checked = false;

                if (str1.Substring(46, 1) == "1")          //厚度判断
                    h_checkBox.Checked = true;
                else
                    h_checkBox.Checked = false;

                if (str1.Substring(49, 1) == "0")           //大小电机判断
                {
                    s_radioButton.Checked = true;
                    b_radioButton.Checked = false;
                }
                else
                {
                    s_radioButton.Checked = false;
                    b_radioButton.Checked = true;
                }

                SN_textBox.Text = str1.Substring(58, 6);    //6位序列号
                Updata_str = str1.Substring(25, 8);
                str1 = "";
            }
            catch
            {
                MessageBox.Show("Serial port error,\r\nPlease check the serial port!!", "error");
            }
        }

        private void Save_button_Click(object sender, EventArgs e)
        {
            Get_button.Enabled = true;
            Save_button.Enabled = false;
            string messer1, messer2, messer3, messer4, messer5;
            if (d_checkBox.Checked == true)     //直径
                messer1 = "1";
            else
                messer1 = "0";

            if (h_checkBox.Checked == true)     //厚度
                messer2 = "1";
            else
                messer2 = "0";

            messer3 = "0";                      // 0德国打印机，1中国打印机
            
            if (s_radioButton.Checked == true)  //大小电机
                messer4 = "0";
            else
                messer4 = "1";

            messer5 = "1";                      // 0恒速，1恒力

            string Set_str = " " + Firmware_textBox.Text + ";" + "000;" + "H  ;" + "21.08.24" + ";E;" + Updata_str + ";0000000000;" + messer1 + messer2 + messer3 + "0" + messer4
                + messer5 + ";0;0;0;" + SN_textBox.Text + ";RWKINSTTBH210       ;" + "\r\n";
            serialPort1.Write("SET\r\n");
            Delay(300);
            serialPort1.Write(Set_str);
            Delay(300);
            serialPort1.Write("END\r\n");
            Set_str = "";
            str1 = "";
            MessageBox.Show("The configuration is saved!", "Succeed");
            //serialPort1.Close();


        }
        public static void Delay(int milliSecond)    // 毫秒延时
        {
            int start = Environment.TickCount;
            while (Math.Abs(Environment.TickCount - start) < milliSecond)//毫秒
            {
                Application.DoEvents();//可执行某无聊的操作
            }
        }
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
          string  str = serialPort1.ReadExisting();
          str1 += str;
       
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            //Search_AddSerialToComboBox(serialPort1, comboBox1);
            Hard_textBox.Enabled = false;
            Firmware_textBox.Enabled = false;
            Brand_textBox.Enabled = false;
            Id_textBox.Enabled = false;
            Save_button.Enabled = false;
        }

        private void button1_Click(object sender, EventArgs e)
        {

            try
            {
                if (button1.Text == "Open")
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.BaudRate = Convert.ToInt32(115200);
                    serialPort1.Open();

                    button1.Text = "Close";
                    button1.BackColor = Color.Green;
                    Get_button.Enabled = true;
                    Get_button.BackColor = Color.Green;
                    Save_button.Enabled = true;
                    Save_button.BackColor = Color.Green;
                }
                else
                {
                    serialPort1.Close();

                    button1.Text = "Open";
                    button1.BackColor = Color.Silver;
                    Get_button.Enabled = false;
                    Get_button.BackColor = Color.Silver;
                    Save_button.Enabled = false;
                    Save_button.BackColor = Color.Silver;
                }
                

            }
            catch
            {
                MessageBox.Show("Serial port error,\r\nPlease check the serial port!!", "error");
                serialPort1.Close();
                // serialPort2.Close();
            }
        }
    }
}
