namespace mdteditor
{
    partial class mdtEditor
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
            System.Windows.Forms.Label label2;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(mdtEditor));
            System.Windows.Forms.Label label3;
            System.Windows.Forms.Label lbBytesAvailable;
            System.Windows.Forms.Label lbSamplesPool;
            System.Windows.Forms.TabControl tabsMain;
            System.Windows.Forms.TabPage tabSamples;
            System.Windows.Forms.Panel panelOpenSave;
            System.Windows.Forms.Panel panelMain;
            System.Windows.Forms.Label lbAlg;
            System.Windows.Forms.Label lbAms;
            System.Windows.Forms.Label lbFms;
            System.Windows.Forms.Label lbAm;
            System.Windows.Forms.Label lbSsgeg;
            System.Windows.Forms.Label lbTl;
            System.Windows.Forms.Label lbDt;
            System.Windows.Forms.Label lbMul;
            System.Windows.Forms.Label lbFb;
            System.Windows.Forms.Label lbPan;
            System.Windows.Forms.Label lbRs;
            System.Windows.Forms.Label lbAr;
            this.txtBytesUsed = new System.Windows.Forms.TextBox();
            this.tabControl_SampleBanks = new System.Windows.Forms.TabControl();
            this.pageSamplesBank1 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank2 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank3 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank4 = new System.Windows.Forms.TabPage();
            this.btnAddSamples = new System.Windows.Forms.Button();
            this.samplesPool = new System.Windows.Forms.Panel();
            this.tabPresets = new System.Windows.Forms.TabPage();
            this.chkReplaceFF = new System.Windows.Forms.CheckBox();
            this.btnAutoLoop = new System.Windows.Forms.Button();
            this.btnAutoRate = new System.Windows.Forms.Button();
            this.btnAutoPan = new System.Windows.Forms.Button();
            this.btnAutoRange = new System.Windows.Forms.Button();
            this.btnSaveRom = new System.Windows.Forms.Button();
            this.btnOpenROM = new System.Windows.Forms.Button();
            this.txtSrmName = new System.Windows.Forms.TextBox();
            this.btnOpenSRM = new System.Windows.Forms.Button();
            this.txtRomName = new System.Windows.Forms.TextBox();
            this.btnSaveSrm = new System.Windows.Forms.Button();
            this.ttCopy = new System.Windows.Forms.ToolTip(this.components);
            this.ttAutoRange = new System.Windows.Forms.ToolTip(this.components);
            this.lbxPresets = new System.Windows.Forms.ListBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.btnLoadPreset = new System.Windows.Forms.Button();
            label2 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            lbBytesAvailable = new System.Windows.Forms.Label();
            lbSamplesPool = new System.Windows.Forms.Label();
            tabsMain = new System.Windows.Forms.TabControl();
            tabSamples = new System.Windows.Forms.TabPage();
            panelOpenSave = new System.Windows.Forms.Panel();
            panelMain = new System.Windows.Forms.Panel();
            lbAlg = new System.Windows.Forms.Label();
            lbAms = new System.Windows.Forms.Label();
            lbFms = new System.Windows.Forms.Label();
            lbAm = new System.Windows.Forms.Label();
            lbSsgeg = new System.Windows.Forms.Label();
            lbTl = new System.Windows.Forms.Label();
            lbDt = new System.Windows.Forms.Label();
            lbMul = new System.Windows.Forms.Label();
            lbFb = new System.Windows.Forms.Label();
            lbPan = new System.Windows.Forms.Label();
            lbRs = new System.Windows.Forms.Label();
            lbAr = new System.Windows.Forms.Label();
            tabsMain.SuspendLayout();
            tabSamples.SuspendLayout();
            this.tabControl_SampleBanks.SuspendLayout();
            this.tabPresets.SuspendLayout();
            panelOpenSave.SuspendLayout();
            panelMain.SuspendLayout();
            this.SuspendLayout();
            // 
            // label2
            // 
            resources.ApplyResources(label2, "label2");
            label2.Name = "label2";
            // 
            // label3
            // 
            resources.ApplyResources(label3, "label3");
            label3.Name = "label3";
            // 
            // lbBytesAvailable
            // 
            resources.ApplyResources(lbBytesAvailable, "lbBytesAvailable");
            lbBytesAvailable.Name = "lbBytesAvailable";
            // 
            // lbSamplesPool
            // 
            resources.ApplyResources(lbSamplesPool, "lbSamplesPool");
            lbSamplesPool.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            lbSamplesPool.Name = "lbSamplesPool";
            // 
            // tabsMain
            // 
            tabsMain.Controls.Add(tabSamples);
            tabsMain.Controls.Add(this.tabPresets);
            resources.ApplyResources(tabsMain, "tabsMain");
            tabsMain.Name = "tabsMain";
            tabsMain.SelectedIndex = 0;
            // 
            // tabSamples
            // 
            tabSamples.BackColor = System.Drawing.Color.Silver;
            tabSamples.Controls.Add(lbBytesAvailable);
            tabSamples.Controls.Add(this.txtBytesUsed);
            tabSamples.Controls.Add(lbSamplesPool);
            tabSamples.Controls.Add(this.tabControl_SampleBanks);
            tabSamples.Controls.Add(this.btnAddSamples);
            tabSamples.Controls.Add(this.samplesPool);
            resources.ApplyResources(tabSamples, "tabSamples");
            tabSamples.Name = "tabSamples";
            // 
            // txtBytesUsed
            // 
            this.txtBytesUsed.BackColor = System.Drawing.Color.WhiteSmoke;
            this.txtBytesUsed.BorderStyle = System.Windows.Forms.BorderStyle.None;
            resources.ApplyResources(this.txtBytesUsed, "txtBytesUsed");
            this.txtBytesUsed.Name = "txtBytesUsed";
            // 
            // tabControl_SampleBanks
            // 
            this.tabControl_SampleBanks.Controls.Add(this.pageSamplesBank1);
            this.tabControl_SampleBanks.Controls.Add(this.pageSamplesBank2);
            this.tabControl_SampleBanks.Controls.Add(this.pageSamplesBank3);
            this.tabControl_SampleBanks.Controls.Add(this.pageSamplesBank4);
            resources.ApplyResources(this.tabControl_SampleBanks, "tabControl_SampleBanks");
            this.tabControl_SampleBanks.Name = "tabControl_SampleBanks";
            this.tabControl_SampleBanks.SelectedIndex = 0;
            // 
            // pageSamplesBank1
            // 
            resources.ApplyResources(this.pageSamplesBank1, "pageSamplesBank1");
            this.pageSamplesBank1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.pageSamplesBank1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank1.Name = "pageSamplesBank1";
            // 
            // pageSamplesBank2
            // 
            resources.ApplyResources(this.pageSamplesBank2, "pageSamplesBank2");
            this.pageSamplesBank2.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.pageSamplesBank2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank2.Name = "pageSamplesBank2";
            // 
            // pageSamplesBank3
            // 
            resources.ApplyResources(this.pageSamplesBank3, "pageSamplesBank3");
            this.pageSamplesBank3.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.pageSamplesBank3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank3.Name = "pageSamplesBank3";
            // 
            // pageSamplesBank4
            // 
            resources.ApplyResources(this.pageSamplesBank4, "pageSamplesBank4");
            this.pageSamplesBank4.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.pageSamplesBank4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank4.Name = "pageSamplesBank4";
            // 
            // btnAddSamples
            // 
            resources.ApplyResources(this.btnAddSamples, "btnAddSamples");
            this.btnAddSamples.Name = "btnAddSamples";
            this.btnAddSamples.UseVisualStyleBackColor = true;
            this.btnAddSamples.Click += new System.EventHandler(this.btnAddSamples_Click);
            // 
            // samplesPool
            // 
            resources.ApplyResources(this.samplesPool, "samplesPool");
            this.samplesPool.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.samplesPool.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.samplesPool.Name = "samplesPool";
            // 
            // tabPresets
            // 
            this.tabPresets.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.tabPresets.Controls.Add(lbAr);
            this.tabPresets.Controls.Add(lbRs);
            this.tabPresets.Controls.Add(lbPan);
            this.tabPresets.Controls.Add(lbFb);
            this.tabPresets.Controls.Add(lbMul);
            this.tabPresets.Controls.Add(lbDt);
            this.tabPresets.Controls.Add(lbTl);
            this.tabPresets.Controls.Add(this.btnLoadPreset);
            this.tabPresets.Controls.Add(lbSsgeg);
            this.tabPresets.Controls.Add(lbAm);
            this.tabPresets.Controls.Add(lbFms);
            this.tabPresets.Controls.Add(lbAms);
            this.tabPresets.Controls.Add(lbAlg);
            this.tabPresets.Controls.Add(this.textBox1);
            this.tabPresets.Controls.Add(this.lbxPresets);
            resources.ApplyResources(this.tabPresets, "tabPresets");
            this.tabPresets.Name = "tabPresets";
            // 
            // panelOpenSave
            // 
            panelOpenSave.BackColor = System.Drawing.Color.Transparent;
            panelOpenSave.Controls.Add(this.chkReplaceFF);
            panelOpenSave.Controls.Add(this.btnAutoLoop);
            panelOpenSave.Controls.Add(this.btnAutoRate);
            panelOpenSave.Controls.Add(this.btnAutoPan);
            panelOpenSave.Controls.Add(this.btnAutoRange);
            panelOpenSave.Controls.Add(this.btnSaveRom);
            panelOpenSave.Controls.Add(this.btnOpenROM);
            panelOpenSave.Controls.Add(this.txtSrmName);
            panelOpenSave.Controls.Add(this.btnOpenSRM);
            panelOpenSave.Controls.Add(this.txtRomName);
            panelOpenSave.Controls.Add(this.btnSaveSrm);
            panelOpenSave.Controls.Add(label3);
            panelOpenSave.Controls.Add(label2);
            panelOpenSave.ForeColor = System.Drawing.SystemColors.AppWorkspace;
            resources.ApplyResources(panelOpenSave, "panelOpenSave");
            panelOpenSave.Name = "panelOpenSave";
            // 
            // chkReplaceFF
            // 
            resources.ApplyResources(this.chkReplaceFF, "chkReplaceFF");
            this.chkReplaceFF.Checked = true;
            this.chkReplaceFF.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkReplaceFF.Name = "chkReplaceFF";
            this.chkReplaceFF.UseVisualStyleBackColor = true;
            // 
            // btnAutoLoop
            // 
            this.btnAutoLoop.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoLoop, "btnAutoLoop");
            this.btnAutoLoop.Name = "btnAutoLoop";
            this.ttCopy.SetToolTip(this.btnAutoLoop, resources.GetString("btnAutoLoop.ToolTip"));
            this.btnAutoLoop.UseVisualStyleBackColor = false;
            this.btnAutoLoop.Click += new System.EventHandler(this.btnAutoLoop_Click);
            // 
            // btnAutoRate
            // 
            this.btnAutoRate.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoRate, "btnAutoRate");
            this.btnAutoRate.Name = "btnAutoRate";
            this.ttCopy.SetToolTip(this.btnAutoRate, resources.GetString("btnAutoRate.ToolTip"));
            this.btnAutoRate.UseVisualStyleBackColor = false;
            this.btnAutoRate.Click += new System.EventHandler(this.btnAutoRate_Click);
            // 
            // btnAutoPan
            // 
            this.btnAutoPan.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoPan, "btnAutoPan");
            this.btnAutoPan.Name = "btnAutoPan";
            this.ttCopy.SetToolTip(this.btnAutoPan, resources.GetString("btnAutoPan.ToolTip"));
            this.btnAutoPan.UseVisualStyleBackColor = false;
            this.btnAutoPan.Click += new System.EventHandler(this.btnAutoPan_Click);
            // 
            // btnAutoRange
            // 
            this.btnAutoRange.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            resources.ApplyResources(this.btnAutoRange, "btnAutoRange");
            this.btnAutoRange.Name = "btnAutoRange";
            this.ttAutoRange.SetToolTip(this.btnAutoRange, resources.GetString("btnAutoRange.ToolTip"));
            this.btnAutoRange.UseVisualStyleBackColor = false;
            this.btnAutoRange.Click += new System.EventHandler(this.btnAutoRange_Click);
            // 
            // btnSaveRom
            // 
            this.btnSaveRom.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(64)))));
            resources.ApplyResources(this.btnSaveRom, "btnSaveRom");
            this.btnSaveRom.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnSaveRom.Name = "btnSaveRom";
            this.btnSaveRom.UseVisualStyleBackColor = false;
            this.btnSaveRom.Click += new System.EventHandler(this.btnSaveRom_Click);
            // 
            // btnOpenROM
            // 
            this.btnOpenROM.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(64)))));
            resources.ApplyResources(this.btnOpenROM, "btnOpenROM");
            this.btnOpenROM.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnOpenROM.Name = "btnOpenROM";
            this.btnOpenROM.UseVisualStyleBackColor = false;
            this.btnOpenROM.Click += new System.EventHandler(this.btnOpenROM_Click);
            // 
            // txtSrmName
            // 
            this.txtSrmName.BackColor = System.Drawing.Color.DimGray;
            this.txtSrmName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtSrmName.ForeColor = System.Drawing.Color.Gainsboro;
            resources.ApplyResources(this.txtSrmName, "txtSrmName");
            this.txtSrmName.Name = "txtSrmName";
            // 
            // btnOpenSRM
            // 
            this.btnOpenSRM.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(7)))));
            resources.ApplyResources(this.btnOpenSRM, "btnOpenSRM");
            this.btnOpenSRM.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnOpenSRM.Name = "btnOpenSRM";
            this.btnOpenSRM.UseVisualStyleBackColor = false;
            this.btnOpenSRM.Click += new System.EventHandler(this.btnOpenSRM_Click);
            // 
            // txtRomName
            // 
            this.txtRomName.BackColor = System.Drawing.Color.DimGray;
            this.txtRomName.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtRomName.ForeColor = System.Drawing.Color.Gainsboro;
            resources.ApplyResources(this.txtRomName, "txtRomName");
            this.txtRomName.Name = "txtRomName";
            // 
            // btnSaveSrm
            // 
            this.btnSaveSrm.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(7)))));
            resources.ApplyResources(this.btnSaveSrm, "btnSaveSrm");
            this.btnSaveSrm.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            this.btnSaveSrm.Name = "btnSaveSrm";
            this.btnSaveSrm.UseVisualStyleBackColor = false;
            this.btnSaveSrm.Click += new System.EventHandler(this.btnSaveSrm_Click);
            // 
            // panelMain
            // 
            resources.ApplyResources(panelMain, "panelMain");
            panelMain.Controls.Add(tabsMain);
            panelMain.Name = "panelMain";
            // 
            // ttCopy
            // 
            this.ttCopy.ToolTipTitle = "Copy from C-0 to all.";
            // 
            // ttAutoRange
            // 
            this.ttAutoRange.ToolTipTitle = "Set auto range";
            // 
            // lbxPresets
            // 
            this.lbxPresets.FormattingEnabled = true;
            resources.ApplyResources(this.lbxPresets, "lbxPresets");
            this.lbxPresets.Name = "lbxPresets";
            // 
            // textBox1
            // 
            resources.ApplyResources(this.textBox1, "textBox1");
            this.textBox1.Name = "textBox1";
            // 
            // lbAlg
            // 
            resources.ApplyResources(lbAlg, "lbAlg");
            lbAlg.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbAlg.Name = "lbAlg";
            // 
            // lbAms
            // 
            resources.ApplyResources(lbAms, "lbAms");
            lbAms.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbAms.Name = "lbAms";
            // 
            // lbFms
            // 
            resources.ApplyResources(lbFms, "lbFms");
            lbFms.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbFms.Name = "lbFms";
            // 
            // lbAm
            // 
            resources.ApplyResources(lbAm, "lbAm");
            lbAm.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbAm.Name = "lbAm";
            // 
            // lbSsgeg
            // 
            resources.ApplyResources(lbSsgeg, "lbSsgeg");
            lbSsgeg.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbSsgeg.Name = "lbSsgeg";
            // 
            // btnLoadPreset
            // 
            this.btnLoadPreset.BackColor = System.Drawing.Color.Purple;
            resources.ApplyResources(this.btnLoadPreset, "btnLoadPreset");
            this.btnLoadPreset.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnLoadPreset.Name = "btnLoadPreset";
            this.btnLoadPreset.UseVisualStyleBackColor = false;
            // 
            // lbTl
            // 
            resources.ApplyResources(lbTl, "lbTl");
            lbTl.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbTl.Name = "lbTl";
            // 
            // lbDt
            // 
            resources.ApplyResources(lbDt, "lbDt");
            lbDt.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbDt.Name = "lbDt";
            // 
            // lbMul
            // 
            resources.ApplyResources(lbMul, "lbMul");
            lbMul.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbMul.Name = "lbMul";
            // 
            // lbFb
            // 
            resources.ApplyResources(lbFb, "lbFb");
            lbFb.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbFb.Name = "lbFb";
            // 
            // lbPan
            // 
            resources.ApplyResources(lbPan, "lbPan");
            lbPan.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbPan.Name = "lbPan";
            // 
            // lbRs
            // 
            resources.ApplyResources(lbRs, "lbRs");
            lbRs.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbRs.Name = "lbRs";
            // 
            // lbAr
            // 
            resources.ApplyResources(lbAr, "lbAr");
            lbAr.ForeColor = System.Drawing.Color.WhiteSmoke;
            lbAr.Name = "lbAr";
            // 
            // mdtEditor
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(10)))), ((int)(((byte)(20)))));
            this.BackgroundImage = global::mdteditor.Properties.Resources.mdtracker_logo2;
            this.Controls.Add(panelMain);
            this.Controls.Add(panelOpenSave);
            this.DoubleBuffered = true;
            this.ForeColor = System.Drawing.SystemColors.ControlText;
            this.Name = "mdtEditor";
            this.Opacity = 0.95D;
            this.ttAutoRange.SetToolTip(this, resources.GetString("$this.ToolTip"));
            this.Load += new System.EventHandler(this.Form1_Load);
            tabsMain.ResumeLayout(false);
            tabSamples.ResumeLayout(false);
            tabSamples.PerformLayout();
            this.tabControl_SampleBanks.ResumeLayout(false);
            this.tabPresets.ResumeLayout(false);
            this.tabPresets.PerformLayout();
            panelOpenSave.ResumeLayout(false);
            panelOpenSave.PerformLayout();
            panelMain.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion
        private System.Windows.Forms.Button btnOpenROM;
        private System.Windows.Forms.Button btnOpenSRM;
        private System.Windows.Forms.Button btnSaveRom;
        private System.Windows.Forms.Button btnSaveSrm;
        private System.Windows.Forms.TabPage pageSamplesBank2;
        private System.Windows.Forms.TabPage pageSamplesBank3;
        private System.Windows.Forms.TabPage pageSamplesBank4;
        private System.Windows.Forms.TextBox txtRomName;
        private System.Windows.Forms.TextBox txtSrmName;
        private System.Windows.Forms.Panel samplesPool;
        private System.Windows.Forms.Button btnAddSamples;
        private System.Windows.Forms.TabPage tabPresets;
        private System.Windows.Forms.TextBox txtBytesUsed;
        private System.Windows.Forms.TabPage pageSamplesBank1;
        private System.Windows.Forms.Button btnAutoRange;
        private System.Windows.Forms.Button btnAutoLoop;
        private System.Windows.Forms.Button btnAutoRate;
        private System.Windows.Forms.Button btnAutoPan;
        private System.Windows.Forms.ToolTip ttCopy;
        private System.Windows.Forms.ToolTip ttAutoRange;
        private System.Windows.Forms.TabControl tabControl_SampleBanks;
        private System.Windows.Forms.CheckBox chkReplaceFF;
        private System.Windows.Forms.ListBox lbxPresets;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button btnLoadPreset;
    }
}

