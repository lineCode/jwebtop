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
using System.Threading;
namespace JWebTop_CSharp_Demo {
    public partial class MainForm : Form, WithinSwingCtrlHelper {
        private DemoBrowserCtrl ctrl = null;
        private JWebTopBrowser listBrowser = null;
        private JWebTopBrowser detailBrowser = null;
        public MainForm() {
            InitializeComponent();
        }

        private void MainForm_Load(object sender, EventArgs e) {
            this.ctrl = new DemoBrowserCtrl();
            this.ctrl.setWithinSwingCtrlHelper(this);
            this.listBrowser = new JWebTopBrowser();
            this.detailBrowser = new JWebTopBrowser();
     
            JWebTopNative.setJsonHandler(this.ctrl);
            listBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            detailBrowser.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitMain.Panel1.Controls.Add(listBrowser);
            this.splitMain.Panel2.Controls.Add(detailBrowser);
            // ע�⣺һ��Ҫ�����߳��н��д���������������C#���������ѵ�Form.Load�¼��Ǵ�WndProc�з����ģ�Ȼ���JWebTopDLL��sendMessage��ͻ�ˣ�������
            Thread th = new Thread(new ThreadStart(createBrowser)); //Ҳ�ɼ�дΪnew Thread(ThreadMethod);                
            th.Start(); //�����߳�  
        }

        private void createBrowser() {
            string appfile = "res/list/index.app";
            JWebTopNative.createJWebTop("JWebTop.exe", appfile);
            listBrowser.createInernalBrowser(appfile, null, null, null);
            appfile = "res/detail/index.app";
            detailBrowser.createInernalBrowser(appfile, null, null, null);
        }

        private void btnNewNote_Click(object sender, EventArgs e) {
            String name = InputBox.ShowInputBox("����", "���������ƣ�");
            name = name.Trim();
            if (name.Length == 0) return;
            ctrl.addNote(name);
        }

        private void btnDelNote_Click(object sender, EventArgs e) {
            String note = ctrl.getCurrentNote();
            if (note == null) return;
            if (MessageBox.Show(this, "�Ƿ�ɾ����" + note + "���ռǣ�") != DialogResult.OK) return;
            ctrl.delNote();
        }

        public int getListHandler() {
            return this.listBrowser.getBrowserHWnd();
        }

        public int getDetailHWnd() {
            return this.detailBrowser.getBrowserHWnd();
        }
    }
}
