using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using JWebTop;
namespace JWebTop_CSharp_Demo
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
        }

        private void btnNewNote_Click(object sender, EventArgs e)
        {
            JWebTopConfigs configs = new JWebTopConfigs();
            configs.appDefFile = "d:/index.app";
            JObject jo = JObject.FromObject(configs);
            string jsonstring = jo.ToString();
            textBox1.Text = jsonstring;
            string v1 = (string)jo["appDefFile"];
            textBox1.Text = textBox1.Text + "\r\n v1=" + v1;
            JWebTopConfigs.removeDefaults(jo);
            String newstr = jo.ToString();
            textBox1.Text = textBox1.Text + "\r\n newstr==" + newstr;
        }
        DemoBrowserCtrl ctrl;
        private void MainForm_Load(object sender, EventArgs e) {
            this.ctrl = new DemoBrowserCtrl();
            JWebTopNative.setJsonHandler(this.ctrl);
            //JWebTopNative.check();// ����ͨ��
            //string a = "����csharp��";
            //int i1 = 10; int i2 = JWebTopNative.Calc(i1);// ����ͨ��
            //a = a + i2
            //StringBuilder sb = new StringBuilder(a);
            //string b = JWebTopNative.CovWString(sb);
            //StringBuilder b = JWebTopNative.CovWString(sb);
            //string b = JWebTopNative.CovWString(a);
            //int b = JWebTopNative.CovString(sb);// ����ͨ��
            //StringBuilder b = JWebTopNative.CovString2(sb);// �����ˣ������õ���ascii[gbk]����������string���ͣ������ǽ�����ԣ�������
            //int b = JWebTopNative.CovWString2(sb);// ����ͨ��
            // b = JWebTopNative.CovWString3(a);// ����ͨ��
            //string b = JWebTopNative.CovWString4(a);// ������
            //string tmp_unicode_error = JWebTopNative.CovWString4(a);// ����������
            //byte[] buffer = Encoding.Unicode.GetBytes(tmp_unicode_error);
            //string t = Encoding.UTF8.GetString(buffer);// �����Ѿ�ת������
            //int c = JWebTopNative.CovWString3("����", "xyz");
            //a = a + t;
            //a = a + b + c;
            //textBox1.Text = a;
            JWebTopNative.createJWebTop("JWebTop.exe", "index.app");
        }
    }
}
