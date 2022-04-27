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
            this.tabControl_SampleBanks = new System.Windows.Forms.TabControl();
            this.pageSamplesBank1 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank2 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank3 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank4 = new System.Windows.Forms.TabPage();
            this.txtBytesUsed = new System.Windows.Forms.TextBox();
            this.btnAddSamples = new System.Windows.Forms.Button();
            this.samplesPool = new System.Windows.Forms.Panel();
            this.tabPresets = new System.Windows.Forms.TabPage();
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
            this.chkReplaceFF = new System.Windows.Forms.CheckBox();
            label2 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            lbBytesAvailable = new System.Windows.Forms.Label();
            lbSamplesPool = new System.Windows.Forms.Label();
            tabsMain = new System.Windows.Forms.TabControl();
            tabSamples = new System.Windows.Forms.TabPage();
            panelOpenSave = new System.Windows.Forms.Panel();
            panelMain = new System.Windows.Forms.Panel();
            this.tabControl_SampleBanks.SuspendLayout();
            tabsMain.SuspendLayout();
            tabSamples.SuspendLayout();
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
            this.pageSamplesBank1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank1.Name = "pageSamplesBank1";
            this.pageSamplesBank1.UseVisualStyleBackColor = true;
            // 
            // pageSamplesBank2
            // 
            resources.ApplyResources(this.pageSamplesBank2, "pageSamplesBank2");
            this.pageSamplesBank2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank2.Name = "pageSamplesBank2";
            this.pageSamplesBank2.UseVisualStyleBackColor = true;
            // 
            // pageSamplesBank3
            // 
            resources.ApplyResources(this.pageSamplesBank3, "pageSamplesBank3");
            this.pageSamplesBank3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank3.Name = "pageSamplesBank3";
            this.pageSamplesBank3.UseVisualStyleBackColor = true;
            // 
            // pageSamplesBank4
            // 
            resources.ApplyResources(this.pageSamplesBank4, "pageSamplesBank4");
            this.pageSamplesBank4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pageSamplesBank4.Name = "pageSamplesBank4";
            this.pageSamplesBank4.UseVisualStyleBackColor = true;
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
            tabSamples.Controls.Add(lbBytesAvailable);
            tabSamples.Controls.Add(this.txtBytesUsed);
            tabSamples.Controls.Add(lbSamplesPool);
            tabSamples.Controls.Add(this.tabControl_SampleBanks);
            tabSamples.Controls.Add(this.btnAddSamples);
            tabSamples.Controls.Add(this.samplesPool);
            resources.ApplyResources(tabSamples, "tabSamples");
            tabSamples.Name = "tabSamples";
            tabSamples.UseVisualStyleBackColor = true;
            // 
            // txtBytesUsed
            // 
            this.txtBytesUsed.BorderStyle = System.Windows.Forms.BorderStyle.None;
            resources.ApplyResources(this.txtBytesUsed, "txtBytesUsed");
            this.txtBytesUsed.Name = "txtBytesUsed";
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
            this.samplesPool.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.samplesPool.Name = "samplesPool";
            // 
            // tabPresets
            // 
            resources.ApplyResources(this.tabPresets, "tabPresets");
            this.tabPresets.Name = "tabPresets";
            this.tabPresets.UseVisualStyleBackColor = true;
            // 
            // panelOpenSave
            // 
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
            resources.ApplyResources(panelOpenSave, "panelOpenSave");
            panelOpenSave.Name = "panelOpenSave";
            // 
            // btnAutoLoop
            // 
            resources.ApplyResources(this.btnAutoLoop, "btnAutoLoop");
            this.btnAutoLoop.Name = "btnAutoLoop";
            this.ttCopy.SetToolTip(this.btnAutoLoop, resources.GetString("btnAutoLoop.ToolTip"));
            this.btnAutoLoop.UseVisualStyleBackColor = true;
            this.btnAutoLoop.Click += new System.EventHandler(this.btnAutoLoop_Click);
            // 
            // btnAutoRate
            // 
            resources.ApplyResources(this.btnAutoRate, "btnAutoRate");
            this.btnAutoRate.Name = "btnAutoRate";
            this.ttCopy.SetToolTip(this.btnAutoRate, resources.GetString("btnAutoRate.ToolTip"));
            this.btnAutoRate.UseVisualStyleBackColor = true;
            this.btnAutoRate.Click += new System.EventHandler(this.btnAutoRate_Click);
            // 
            // btnAutoPan
            // 
            resources.ApplyResources(this.btnAutoPan, "btnAutoPan");
            this.btnAutoPan.Name = "btnAutoPan";
            this.ttCopy.SetToolTip(this.btnAutoPan, resources.GetString("btnAutoPan.ToolTip"));
            this.btnAutoPan.UseVisualStyleBackColor = true;
            this.btnAutoPan.Click += new System.EventHandler(this.btnAutoPan_Click);
            // 
            // btnAutoRange
            // 
            resources.ApplyResources(this.btnAutoRange, "btnAutoRange");
            this.btnAutoRange.Name = "btnAutoRange";
            this.ttAutoRange.SetToolTip(this.btnAutoRange, resources.GetString("btnAutoRange.ToolTip"));
            this.btnAutoRange.UseVisualStyleBackColor = true;
            this.btnAutoRange.Click += new System.EventHandler(this.btnAutoRange_Click);
            // 
            // btnSaveRom
            // 
            resources.ApplyResources(this.btnSaveRom, "btnSaveRom");
            this.btnSaveRom.Name = "btnSaveRom";
            this.btnSaveRom.UseVisualStyleBackColor = true;
            this.btnSaveRom.Click += new System.EventHandler(this.btnSaveRom_Click);
            // 
            // btnOpenROM
            // 
            resources.ApplyResources(this.btnOpenROM, "btnOpenROM");
            this.btnOpenROM.Name = "btnOpenROM";
            this.btnOpenROM.UseVisualStyleBackColor = true;
            this.btnOpenROM.Click += new System.EventHandler(this.btnOpenROM_Click);
            // 
            // txtSrmName
            // 
            resources.ApplyResources(this.txtSrmName, "txtSrmName");
            this.txtSrmName.Name = "txtSrmName";
            // 
            // btnOpenSRM
            // 
            resources.ApplyResources(this.btnOpenSRM, "btnOpenSRM");
            this.btnOpenSRM.Name = "btnOpenSRM";
            this.btnOpenSRM.UseVisualStyleBackColor = true;
            this.btnOpenSRM.Click += new System.EventHandler(this.btnOpenSRM_Click);
            // 
            // txtRomName
            // 
            resources.ApplyResources(this.txtRomName, "txtRomName");
            this.txtRomName.Name = "txtRomName";
            // 
            // btnSaveSrm
            // 
            resources.ApplyResources(this.btnSaveSrm, "btnSaveSrm");
            this.btnSaveSrm.Name = "btnSaveSrm";
            this.btnSaveSrm.UseVisualStyleBackColor = true;
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
            // chkReplaceFF
            // 
            resources.ApplyResources(this.chkReplaceFF, "chkReplaceFF");
            this.chkReplaceFF.Checked = true;
            this.chkReplaceFF.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkReplaceFF.Name = "chkReplaceFF";
            this.chkReplaceFF.UseVisualStyleBackColor = true;
            // 
            // mdtEditor
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(panelMain);
            this.Controls.Add(panelOpenSave);
            this.Name = "mdtEditor";
            this.ttAutoRange.SetToolTip(this, resources.GetString("$this.ToolTip"));
            this.Load += new System.EventHandler(this.Form1_Load);
            this.tabControl_SampleBanks.ResumeLayout(false);
            tabsMain.ResumeLayout(false);
            tabSamples.ResumeLayout(false);
            tabSamples.PerformLayout();
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
    }
}

