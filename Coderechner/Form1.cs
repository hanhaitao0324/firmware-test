using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;


namespace ERWEKEA_Coderechner
{
    
    public partial class Form1 : Form
    {
        string Copytext;
        string Passwordstr;
        string LastTextbox;
        long temp_num;
        int num_button = 0;
        long[] Keybuff = new long[]
                          {
                           934769, 456434, 338702, 617318, 737614,
                           744255, 719715, 244117, 837043, 756451,
                           934141, 127039, 580191, 323951, 980898,
                           621566, 935678, 114353, 521098, 980640,
                           144494, 382308, 109701, 140036, 642865,
                           408337, 721120, 940467, 454553, 971846,
                           969139, 453618, 919225, 629771, 711496,
                           464426, 189122, 716139, 990195, 656344
                          };//40组6位的随机数

        string[] Passwordkey = new string[20];//存放激活码的数组
        public Form1()
        {
            InitializeComponent();
        }

        public void Password(int key_num ,long num)             
        {
            for(int i = 0;i< 16;i++)
            {
                temp_num = 0;
                Passwordstr = "";

                temp_num = Keybuff[key_num] ^ num;
                string str1 = temp_num.ToString();
                if (str1.Length >= 6)
                {
                    char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                    Passwordstr = string.Join("", mychar);
                }
                else
                {
                    int mychar_num = 5;
                    char[] mychar = new char[6];
                    for (int k = str1.Length - 1; k >= 0; k--)
                    {

                        mychar[mychar_num] = str1[k];
                        mychar_num--;
                    }
                    while (true)
                    {
                        mychar[mychar_num] = '0';
                        mychar_num--;
                        if (mychar_num < 0)
                            break;
                    }
                    Passwordstr = string.Join("", mychar);
                }
                key_num++;
                Passwordkey[i] = Passwordstr;
                
            }
            

        }
        public static string Choose(int switch_num,long num)
        {
            string str2 = "" ;
            long num_six ;
            long id_num = 0;
            if (switch_num > 15)
                switch_num = switch_num-16;
            switch (switch_num)
            {
                case 0:
                    {
                        num_six = 499530;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length-1; k>=0;k--)
                            {
                                
                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 1:
                    {
                        num_six = 566640;
                        id_num = num ^ num_six;
                        
                        
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 2:
                    {
                        num_six = 752395;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 3:
                    {
                        num_six = 120907;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 4:
                    {
                        num_six = 471186;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 5:
                    {
                        num_six = 100345;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 6:
                    {
                        num_six = 362137;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 7:
                    {
                        num_six = 742371;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 8:
                    {
                        num_six = 290819;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;

                case 9:
                    {
                        num_six = 500840;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 10:
                    {
                        num_six = 689457;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 11:
                    {
                        num_six = 241507;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 12:
                    {
                        num_six = 315809;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 13:
                    {
                        num_six = 755724;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 14:
                    {
                        num_six = 800502;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                case 15:
                    {
                        num_six = 285197;
                        id_num = num ^ num_six;
                        string str1 = id_num.ToString();
                        if (str1.Length >= 6)
                        {
                            char[] mychar = { str1[str1.Length - 6], str1[str1.Length - 5], str1[str1.Length - 4], str1[str1.Length - 3], str1[str1.Length - 2], str1[str1.Length - 1] };
                            str2 = string.Join("", mychar);
                        }
                        else
                        {
                            int mychar_num = 5;
                            char[] mychar = new char[6];
                            for (int k = str1.Length - 1; k >= 0; k--)
                            {

                                mychar[mychar_num] = str1[k];
                                mychar_num--;
                            }
                            while (true)
                            {
                                mychar[mychar_num] = '0';
                                mychar_num--;
                                if (mychar_num < 0)
                                    break;
                            }
                            str2 = string.Join("", mychar);
                        }
                    }
                    break;
                default:
                    break;
            }
            return str2;
        }
        public static int Chance_num(char mychar)
        {
            int i = 17;
            switch (mychar)
            {
                case '0':
                    {
                        i = 0;
                    }
                    break;
                case '1':
                    {
                        i = 1;
                    }
                    break;
                case '2':
                    {
                        i = 2;
                    }
                    break;
                case '3':
                    {
                        i = 3;
                    }
                    break;
                case '4':
                    {
                        i = 4;
                    }
                    break;
                case '5':
                    {
                        i = 5;
                    }
                    break;
                case '6':
                    {
                        i = 6;
                    }
                    break;
                case '7':
                    {
                        i = 7;
                    }
                    break;
                case '8':
                    {
                        i = 8;
                    }
                    break;

                case '9':
                    {
                        i = 9;
                    }
                    break;
                case 'A':
                    {
                        i = 10;
                    }
                    break;
                case 'B':
                    {
                        i = 11;
                    }
                    break;
                case 'C':
                    {
                        i = 12;
                    }
                    break;
                case 'D':
                    {
                        i = 13;
                    }
                    break;
                case 'E':
                    {
                        i = 14;
                    }
                    break;
                case 'F':
                    {
                        i = 15;
                    }
                    break;
                default:
                    break;
            }
            return i;
        }

        private void id_textBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            if (!(Char.IsNumber(e.KeyChar)) && e.KeyChar != (char)8 )
                e.Handled = true;
        }

        private void id_textBox_TextChanged(object sender, EventArgs e)
        {
            if (this.id_textBox.Text.Length == 10)
            {
                label1.Visible = false;
            }
            else
            {
                label1.Visible = true;
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button121_Click(object sender, EventArgs e)
        {
            if (textBox121.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox121.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button122_Click(object sender, EventArgs e)
        {
            if (textBox122.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox122.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button221_Click(object sender, EventArgs e)
        {
            if (textBox221.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox221.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button222_Click(object sender, EventArgs e)
        {
            if (textBox222.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox222.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button223_Click(object sender, EventArgs e)
        {
            if (textBox223.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox223.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void scpr_button_Click(object sender, EventArgs e)
        {
            if (comboBox_SPR.Text != "")
            {
                Clipboard.Clear();
                Copytext = comboBox_SPR.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_SvmC_Click(object sender, EventArgs e)
        {
            if (id_textBox.Text.Length == 10)
            {
                if (string.Equals(id_textBox.Text, LastTextbox) == true)
                {
                    if (num_button > 14)
                        MessageBox.Show("It's already the last group", "ERR0R");
                    else
                    {
                        num_button++;
                        comboBox_SPR.Items.Add(Passwordkey[num_button]);          //添加到下拉框
                        comboBox_SPR.Text = Passwordkey[num_button];              //默认显示下一组  
                    }
                }
                else
                {
                    num_button = 0;
                    LastTextbox = id_textBox.Text;
                    byte[] Data = new byte[4];                                 //存放第一个字节前五位
                    int[] num_save = new int[5];
                    long num;                                                   //存放输入的10位数
                    long.TryParse(id_textBox.Text, out num);                    //将字符型十进制转换成普通十进制数
                    string str = num.ToString("X2");                            // str转换后是702B6961

                    textBox121.Text = Choose(Chance_num(str[0]), num);
                    textBox122.Text = Choose(Chance_num(str[0]) + 1, num);
                    textBox221.Text = Choose(Chance_num(str[0]) + 2, num);
                    textBox222.Text = Choose(Chance_num(str[0]) + 3, num);
                    textBox223.Text = Choose(Chance_num(str[0]) + 4, num);

                    comboBox_SPR.Items.Clear();                                  //清空下拉框
                    Password(Chance_num(str[0]), num);                          //20组密码存在Passwordkey数组中
                    comboBox_SPR.Items.Add(Passwordkey[0]);                      //添加到下拉框
                    comboBox_SPR.Text = Passwordkey[0];                          //默认初始化第一个
                }
            }
            else
            {
                MessageBox.Show("Instrument ID Must be ten", "ERR0R");
            }
        }

        private void button_TSD_Click(object sender, EventArgs e)
        {
            if (textBox_TFD.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox_TFD.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_TFDB_Click(object sender, EventArgs e)
        {
            if (textBox_TFDB.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox_TFDB.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_TSD_Click_1(object sender, EventArgs e)
        {
            if (textBox_TSD.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox_TSD.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_TSDB_Click(object sender, EventArgs e)
        {
            if (textBox_TSDB.Text != "")
            {
                Clipboard.Clear();
                Copytext = textBox_TSDB.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_TPR_Click(object sender, EventArgs e)
        {
            if (comboBox_TPR.Text != "")
            {
                Clipboard.Clear();
                Copytext = comboBox_TPR.Text;
                Clipboard.SetText(Copytext);
            }
        }

        private void button_TarC_Click(object sender, EventArgs e)
        {
            if (id_textBox.Text.Length == 10)
            {
                if (string.Equals(id_textBox.Text, LastTextbox) == true)
                {
                    if (num_button > 14)
                        MessageBox.Show("It's already the last group", "ERR0R");
                    else
                    {
                        num_button++;
                        comboBox_TPR.Items.Add(Passwordkey[num_button]);          //添加到下拉框
                        comboBox_TPR.Text = Passwordkey[num_button];              //默认显示下一组  
                    }
                }
                else
                {
                    num_button = 0;
                    LastTextbox = id_textBox.Text;
                    byte[] Data = new byte[4];                                 //存放第一个字节前五位
                    int[] num_save = new int[5];
                    long num;                                                   //存放输入的10位数
                    long.TryParse(id_textBox.Text, out num);                    //将字符型十进制转换成普通十进制数
                    string str = num.ToString("X2");                            // str转换后是702B6961

                    textBox_TFD.Text = Choose(Chance_num(str[0]), num);
                    textBox_TFDB.Text = Choose(Chance_num(str[0]) + 1, num);
                    textBox_TSD.Text = Choose(Chance_num(str[0]) + 2, num);
                    textBox_TSDB.Text = Choose(Chance_num(str[0]) + 3, num);
                    //gn5_textBox.Text = Choose(Chance_num(str[0]) + 4, num);

                    comboBox_TPR.Items.Clear();                                  //清空下拉框
                    Password(Chance_num(str[0]), num);                          //20组密码存在Passwordkey数组中
                    comboBox_TPR.Items.Add(Passwordkey[0]);                      //添加到下拉框
                    comboBox_TPR.Text = Passwordkey[0];                          //默认初始化第一个
                }
            }
            else
            {
                MessageBox.Show("Instrument ID Must be ten", "ERR0R");
            }
        }

    }
}
