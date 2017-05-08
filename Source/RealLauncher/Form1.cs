using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;

namespace RealLauncher
{
    public partial class Form1 : Form
    {
        //LauncherBridge myLauncher = new LauncherBridge();
        Screen myScreen = new Screen();
        //System.Threading.Thread myBrontoSaurusThread;
        //bool running = false;
        public Form1()
        {
            InitializeComponent();
            myScreen.FillResulotions();
            //timer2.Start();
            PlayButtonClick.Visible = false;
            for (int i = 0; i < myScreen.myResolutions.Count; i++)
            {
                if (!comboBox1.Items.Contains(myScreen.myResolutions[i].width.ToString() + "X" + myScreen.myResolutions[i].height.ToString()))
                {
                    comboBox1.Items.Add(myScreen.myResolutions[i].width.ToString() + "X" + myScreen.myResolutions[i].height.ToString());
                }
                else
                {
                    myScreen.myResolutions.RemoveAt(i);
                    i--;
                }
            }

            for (int i = 0; i < comboBox1.Items.Count; i++)
            {
                if (comboBox1.Items[i].ToString() == "1920X1080")
                {
                    comboBox1.SelectedItem = comboBox1.Items[i];
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            //if (running == true)
            //{
            //    return;
            //}
            //int wh = 0;
            //int ww = 0;

            //wh = myScreen.myResolutions[comboBox1.SelectedIndex].height;
            //ww = myScreen.myResolutions[comboBox1.SelectedIndex].width;
            //myBrontoSaurusThread = new Thread(() => myLauncher.Launch(wh, ww, fullscreen));
            //myBrontoSaurusThread.Start();
            //timer1.Start();
            //running = true;
            int wh = 0;
            int ww = 0;
            bool fullscreen = FullScreen.Checked;

            wh = myScreen.myResolutions[comboBox1.SelectedIndex].height;
            ww = myScreen.myResolutions[comboBox1.SelectedIndex].width;

            //ProcessStartInfo startinfo1 = new ProcessStartInfo();
            //startinfo1.CreateNoWindow = false;
            //startinfo1.UseShellExecute = false;
            //startinfo1.FileName = "TServer_Applictaion_x64_Release.exe";
            ////startinfo.Arguments = "-height " + wh + " -width " + ww + " -fullscreen " + fullscreen;
            //Process.Start(startinfo1);


            ProcessStartInfo startinfo = new ProcessStartInfo();
            startinfo.CreateNoWindow = false;
            startinfo.UseShellExecute = false;
            startinfo.FileName = "./HighDoom.exe";
            startinfo.Arguments = "-height " + wh + " -width " + ww + " -fullscreen " + fullscreen;
            Process.Start(startinfo);

            this.Close();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            //if (myBrontoSaurusThread.IsAlive != true)
            //{
            //    this.Close();
            //}
        }

        private void PlayButtonDefault_MouseEnter(object sender, EventArgs e)
        {
            if(PlayButtonClick.Visible == false)
            {
                PlayButtonClick.Visible = true;
                PlayButtonClick.Refresh();
            }
            
        }

        private void PlayButtonDefault_MouseLeave(object sender, EventArgs e)
        {
            PlayButtonClick.Visible = false;
        }

        private void PlayButtonClick_MouseDown(object sender, MouseEventArgs e)
        {
            PlayButtonClick.BackgroundImage = ((System.Drawing.Image)(Properties.Resources.play_active));
        }

        private void PlayButtonClick_MouseUp(object sender, MouseEventArgs e)
        {
            PlayButtonClick.BackgroundImage = ((System.Drawing.Image)(Properties.Resources.play_hover));
        }

        //private void timer2_Tick(object sender, EventArgs e)
        //{
        //    button1.Refresh();
        //}

        //private void button1_MouseDown(object sender, MouseEventArgs e)
        //{
        //    button1.BackgroundImage = ((System.Drawing.Image)(Properties.Resources.play_active));
        //}

        //private void button1_MouseUp(object sender, MouseEventArgs e)
        //{
        //    button1.BackgroundImage = ((System.Drawing.Image)(Properties.Resources.play_default));
        //}

        //private void button1_Paint(object sender, PaintEventArgs e)
        //{

        //    Bitmap bmp = Properties.Resources.Untitled_1;
        //    bmp.MakeTransparent(Color.White);
        //    int x = (button1.Width - bmp.Width) / 2;
        //    int y = (button1.Height - bmp.Height) / 2;
        //    e.Graphics.DrawImage(bmp, x, y);
        //}

    }
}
