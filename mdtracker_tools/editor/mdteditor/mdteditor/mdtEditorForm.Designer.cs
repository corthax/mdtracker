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
            System.Windows.Forms.Label lbCopy;
            System.Windows.Forms.Label label5;
            System.Windows.Forms.Panel panelOpenSave;
            System.Windows.Forms.Panel pnAutoRange;
            System.Windows.Forms.Label label4;
            System.Windows.Forms.Label label1;
            System.Windows.Forms.Label lbAssignSRM;
            System.Windows.Forms.Label lbAssignROM;
            System.Windows.Forms.Panel panelMain;
            this.chkReplaceFF = new System.Windows.Forms.CheckBox();
            this.txtBytesUsed = new System.Windows.Forms.TextBox();
            this.btnAutoLoop = new System.Windows.Forms.Button();
            this.tabControl_SampleBanks = new System.Windows.Forms.TabControl();
            this.pageSamplesBank1 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank2 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank3 = new System.Windows.Forms.TabPage();
            this.pageSamplesBank4 = new System.Windows.Forms.TabPage();
            this.btnAutoRate = new System.Windows.Forms.Button();
            this.btnAddSamples = new System.Windows.Forms.Button();
            this.btnAutoPan = new System.Windows.Forms.Button();
            this.pnSamplesPool = new System.Windows.Forms.Panel();
            this.tabPresets = new System.Windows.Forms.TabPage();
            this.txtPresetTarget = new System.Windows.Forms.TextBox();
            this.txtPresetsSource = new System.Windows.Forms.TextBox();
            this.btnAssignPreset = new System.Windows.Forms.Button();
            this.pnPresetsPool = new System.Windows.Forms.Panel();
            this.btnAddPresets = new System.Windows.Forms.Button();
            this.tcInstruments = new System.Windows.Forms.TabControl();
            this.tbInstSRM = new System.Windows.Forms.TabPage();
            this.tbInstROM = new System.Windows.Forms.TabPage();
            this.bankAddress = new System.Windows.Forms.Label();
            this.btnAutoRange = new System.Windows.Forms.Button();
            this.txtAssignStart_ROM = new System.Windows.Forms.TextBox();
            this.txtAssignEnd_ROM = new System.Windows.Forms.TextBox();
            this.txtAssignEnd_SRM = new System.Windows.Forms.TextBox();
            this.txtAssignStart_SRM = new System.Windows.Forms.TextBox();
            this.SampleBankAddress = new System.Windows.Forms.TextBox();
            this.btnSaveRom = new System.Windows.Forms.Button();
            this.btnOpenROM = new System.Windows.Forms.Button();
            this.txtSrmName = new System.Windows.Forms.TextBox();
            this.btnOpenSRM = new System.Windows.Forms.Button();
            this.txtRomName = new System.Windows.Forms.TextBox();
            this.btnSaveSrm = new System.Windows.Forms.Button();
            this.ttCopy = new System.Windows.Forms.ToolTip(this.components);
            this.ttAutoRange = new System.Windows.Forms.ToolTip(this.components);
            this.dacDescription = new System.Windows.Forms.Label();
            label2 = new System.Windows.Forms.Label();
            label3 = new System.Windows.Forms.Label();
            lbBytesAvailable = new System.Windows.Forms.Label();
            lbSamplesPool = new System.Windows.Forms.Label();
            tabsMain = new System.Windows.Forms.TabControl();
            tabSamples = new System.Windows.Forms.TabPage();
            lbCopy = new System.Windows.Forms.Label();
            label5 = new System.Windows.Forms.Label();
            panelOpenSave = new System.Windows.Forms.Panel();
            pnAutoRange = new System.Windows.Forms.Panel();
            label4 = new System.Windows.Forms.Label();
            label1 = new System.Windows.Forms.Label();
            lbAssignSRM = new System.Windows.Forms.Label();
            lbAssignROM = new System.Windows.Forms.Label();
            panelMain = new System.Windows.Forms.Panel();
            tabsMain.SuspendLayout();
            tabSamples.SuspendLayout();
            this.tabControl_SampleBanks.SuspendLayout();
            this.tabPresets.SuspendLayout();
            this.tcInstruments.SuspendLayout();
            panelOpenSave.SuspendLayout();
            pnAutoRange.SuspendLayout();
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
            tabSamples.Controls.Add(lbCopy);
            tabSamples.Controls.Add(this.chkReplaceFF);
            tabSamples.Controls.Add(lbBytesAvailable);
            tabSamples.Controls.Add(this.txtBytesUsed);
            tabSamples.Controls.Add(lbSamplesPool);
            tabSamples.Controls.Add(this.btnAutoLoop);
            tabSamples.Controls.Add(this.tabControl_SampleBanks);
            tabSamples.Controls.Add(this.btnAutoRate);
            tabSamples.Controls.Add(this.btnAddSamples);
            tabSamples.Controls.Add(this.btnAutoPan);
            tabSamples.Controls.Add(this.pnSamplesPool);
            resources.ApplyResources(tabSamples, "tabSamples");
            tabSamples.Name = "tabSamples";
            // 
            // lbCopy
            // 
            resources.ApplyResources(lbCopy, "lbCopy");
            lbCopy.BackColor = System.Drawing.Color.DarkBlue;
            lbCopy.ForeColor = System.Drawing.Color.Gainsboro;
            lbCopy.Name = "lbCopy";
            // 
            // chkReplaceFF
            // 
            resources.ApplyResources(this.chkReplaceFF, "chkReplaceFF");
            this.chkReplaceFF.Checked = true;
            this.chkReplaceFF.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkReplaceFF.Name = "chkReplaceFF";
            this.chkReplaceFF.UseVisualStyleBackColor = true;
            // 
            // txtBytesUsed
            // 
            this.txtBytesUsed.BackColor = System.Drawing.Color.WhiteSmoke;
            this.txtBytesUsed.BorderStyle = System.Windows.Forms.BorderStyle.None;
            resources.ApplyResources(this.txtBytesUsed, "txtBytesUsed");
            this.txtBytesUsed.Name = "txtBytesUsed";
            // 
            // btnAutoLoop
            // 
            this.btnAutoLoop.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoLoop, "btnAutoLoop");
            this.btnAutoLoop.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnAutoLoop.Name = "btnAutoLoop";
            this.ttCopy.SetToolTip(this.btnAutoLoop, resources.GetString("btnAutoLoop.ToolTip"));
            this.btnAutoLoop.UseVisualStyleBackColor = false;
            this.btnAutoLoop.Click += new System.EventHandler(this.btnAutoLoop_Click);
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
            // btnAutoRate
            // 
            this.btnAutoRate.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoRate, "btnAutoRate");
            this.btnAutoRate.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnAutoRate.Name = "btnAutoRate";
            this.ttCopy.SetToolTip(this.btnAutoRate, resources.GetString("btnAutoRate.ToolTip"));
            this.btnAutoRate.UseVisualStyleBackColor = false;
            this.btnAutoRate.Click += new System.EventHandler(this.btnAutoRate_Click);
            // 
            // btnAddSamples
            // 
            resources.ApplyResources(this.btnAddSamples, "btnAddSamples");
            this.btnAddSamples.Name = "btnAddSamples";
            this.btnAddSamples.UseVisualStyleBackColor = true;
            this.btnAddSamples.Click += new System.EventHandler(this.btnAddSamples_Click);
            // 
            // btnAutoPan
            // 
            this.btnAutoPan.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAutoPan, "btnAutoPan");
            this.btnAutoPan.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnAutoPan.Name = "btnAutoPan";
            this.ttCopy.SetToolTip(this.btnAutoPan, resources.GetString("btnAutoPan.ToolTip"));
            this.btnAutoPan.UseVisualStyleBackColor = false;
            this.btnAutoPan.Click += new System.EventHandler(this.btnAutoPan_Click);
            // 
            // pnSamplesPool
            // 
            resources.ApplyResources(this.pnSamplesPool, "pnSamplesPool");
            this.pnSamplesPool.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.pnSamplesPool.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnSamplesPool.Name = "pnSamplesPool";
            this.pnSamplesPool.Paint += new System.Windows.Forms.PaintEventHandler(this.pnSamplesPool_Paint);
            // 
            // tabPresets
            // 
            this.tabPresets.BackColor = System.Drawing.Color.Silver;
            this.tabPresets.Controls.Add(label5);
            this.tabPresets.Controls.Add(this.txtPresetTarget);
            this.tabPresets.Controls.Add(this.txtPresetsSource);
            this.tabPresets.Controls.Add(this.btnAssignPreset);
            this.tabPresets.Controls.Add(this.pnPresetsPool);
            this.tabPresets.Controls.Add(this.btnAddPresets);
            this.tabPresets.Controls.Add(this.tcInstruments);
            resources.ApplyResources(this.tabPresets, "tabPresets");
            this.tabPresets.Name = "tabPresets";
            // 
            // label5
            // 
            label5.BackColor = System.Drawing.Color.MidnightBlue;
            label5.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            label5.ForeColor = System.Drawing.Color.Gainsboro;
            resources.ApplyResources(label5, "label5");
            label5.Name = "label5";
            // 
            // txtPresetTarget
            // 
            this.txtPresetTarget.BackColor = System.Drawing.Color.Silver;
            this.txtPresetTarget.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtPresetTarget, "txtPresetTarget");
            this.txtPresetTarget.Name = "txtPresetTarget";
            // 
            // txtPresetsSource
            // 
            this.txtPresetsSource.BackColor = System.Drawing.Color.Silver;
            this.txtPresetsSource.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtPresetsSource, "txtPresetsSource");
            this.txtPresetsSource.Name = "txtPresetsSource";
            // 
            // btnAssignPreset
            // 
            this.btnAssignPreset.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(5)))), ((int)(((byte)(5)))), ((int)(((byte)(10)))));
            resources.ApplyResources(this.btnAssignPreset, "btnAssignPreset");
            this.btnAssignPreset.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnAssignPreset.Name = "btnAssignPreset";
            this.btnAssignPreset.UseVisualStyleBackColor = false;
            // 
            // pnPresetsPool
            // 
            resources.ApplyResources(this.pnPresetsPool, "pnPresetsPool");
            this.pnPresetsPool.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(30)))), ((int)(((byte)(30)))), ((int)(((byte)(30)))));
            this.pnPresetsPool.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnPresetsPool.Name = "pnPresetsPool";
            // 
            // btnAddPresets
            // 
            resources.ApplyResources(this.btnAddPresets, "btnAddPresets");
            this.btnAddPresets.Name = "btnAddPresets";
            this.btnAddPresets.UseVisualStyleBackColor = true;
            this.btnAddPresets.Click += new System.EventHandler(this.btnAddPresets_Click);
            // 
            // tcInstruments
            // 
            this.tcInstruments.Controls.Add(this.tbInstSRM);
            this.tcInstruments.Controls.Add(this.tbInstROM);
            resources.ApplyResources(this.tcInstruments, "tcInstruments");
            this.tcInstruments.Name = "tcInstruments";
            this.tcInstruments.SelectedIndex = 0;
            // 
            // tbInstSRM
            // 
            resources.ApplyResources(this.tbInstSRM, "tbInstSRM");
            this.tbInstSRM.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.tbInstSRM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbInstSRM.Name = "tbInstSRM";
            // 
            // tbInstROM
            // 
            resources.ApplyResources(this.tbInstROM, "tbInstROM");
            this.tbInstROM.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(64)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.tbInstROM.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbInstROM.Name = "tbInstROM";
            // 
            // panelOpenSave
            // 
            panelOpenSave.BackColor = System.Drawing.Color.Transparent;
            panelOpenSave.Controls.Add(this.bankAddress);
            panelOpenSave.Controls.Add(pnAutoRange);
            panelOpenSave.Controls.Add(this.SampleBankAddress);
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
            // bankAddress
            // 
            resources.ApplyResources(this.bankAddress, "bankAddress");
            this.bankAddress.BackColor = System.Drawing.Color.DarkSlateBlue;
            this.bankAddress.ForeColor = System.Drawing.SystemColors.ButtonHighlight;
            this.bankAddress.Name = "bankAddress";
            // 
            // pnAutoRange
            // 
            pnAutoRange.BackColor = System.Drawing.Color.DarkOrchid;
            pnAutoRange.Controls.Add(this.btnAutoRange);
            pnAutoRange.Controls.Add(label4);
            pnAutoRange.Controls.Add(this.txtAssignStart_ROM);
            pnAutoRange.Controls.Add(label1);
            pnAutoRange.Controls.Add(this.txtAssignEnd_ROM);
            pnAutoRange.Controls.Add(lbAssignSRM);
            pnAutoRange.Controls.Add(this.txtAssignEnd_SRM);
            pnAutoRange.Controls.Add(lbAssignROM);
            pnAutoRange.Controls.Add(this.txtAssignStart_SRM);
            pnAutoRange.ForeColor = System.Drawing.Color.DimGray;
            resources.ApplyResources(pnAutoRange, "pnAutoRange");
            pnAutoRange.Name = "pnAutoRange";
            // 
            // btnAutoRange
            // 
            this.btnAutoRange.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(70)))), ((int)(((byte)(0)))), ((int)(((byte)(0)))));
            resources.ApplyResources(this.btnAutoRange, "btnAutoRange");
            this.btnAutoRange.ForeColor = System.Drawing.Color.Gainsboro;
            this.btnAutoRange.Name = "btnAutoRange";
            this.ttAutoRange.SetToolTip(this.btnAutoRange, resources.GetString("btnAutoRange.ToolTip"));
            this.btnAutoRange.UseVisualStyleBackColor = false;
            this.btnAutoRange.Click += new System.EventHandler(this.btnAutoRange_Click);
            // 
            // label4
            // 
            label4.BackColor = System.Drawing.Color.Black;
            label4.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            label4.ForeColor = System.Drawing.Color.Gold;
            resources.ApplyResources(label4, "label4");
            label4.Name = "label4";
            // 
            // txtAssignStart_ROM
            // 
            this.txtAssignStart_ROM.BackColor = System.Drawing.Color.Silver;
            this.txtAssignStart_ROM.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtAssignStart_ROM, "txtAssignStart_ROM");
            this.txtAssignStart_ROM.Name = "txtAssignStart_ROM";
            this.txtAssignStart_ROM.TextChanged += new System.EventHandler(this.txtAssignStart_ROM_TextChanged);
            // 
            // label1
            // 
            label1.BackColor = System.Drawing.Color.MidnightBlue;
            label1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            label1.ForeColor = System.Drawing.Color.Gainsboro;
            resources.ApplyResources(label1, "label1");
            label1.Name = "label1";
            // 
            // txtAssignEnd_ROM
            // 
            this.txtAssignEnd_ROM.BackColor = System.Drawing.Color.Silver;
            this.txtAssignEnd_ROM.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtAssignEnd_ROM, "txtAssignEnd_ROM");
            this.txtAssignEnd_ROM.Name = "txtAssignEnd_ROM";
            // 
            // lbAssignSRM
            // 
            lbAssignSRM.BackColor = System.Drawing.Color.Black;
            lbAssignSRM.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            lbAssignSRM.ForeColor = System.Drawing.Color.Gold;
            resources.ApplyResources(lbAssignSRM, "lbAssignSRM");
            lbAssignSRM.Name = "lbAssignSRM";
            // 
            // txtAssignEnd_SRM
            // 
            this.txtAssignEnd_SRM.BackColor = System.Drawing.Color.Silver;
            this.txtAssignEnd_SRM.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtAssignEnd_SRM, "txtAssignEnd_SRM");
            this.txtAssignEnd_SRM.Name = "txtAssignEnd_SRM";
            // 
            // lbAssignROM
            // 
            lbAssignROM.BackColor = System.Drawing.Color.MidnightBlue;
            lbAssignROM.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            lbAssignROM.ForeColor = System.Drawing.Color.Gainsboro;
            resources.ApplyResources(lbAssignROM, "lbAssignROM");
            lbAssignROM.Name = "lbAssignROM";
            // 
            // txtAssignStart_SRM
            // 
            this.txtAssignStart_SRM.BackColor = System.Drawing.Color.Silver;
            this.txtAssignStart_SRM.ForeColor = System.Drawing.Color.Black;
            resources.ApplyResources(this.txtAssignStart_SRM, "txtAssignStart_SRM");
            this.txtAssignStart_SRM.Name = "txtAssignStart_SRM";
            // 
            // SampleBankAddress
            // 
            resources.ApplyResources(this.SampleBankAddress, "SampleBankAddress");
            this.SampleBankAddress.Name = "SampleBankAddress";
            this.SampleBankAddress.TextChanged += new System.EventHandler(this.SampleBankAddress_TextChanged);
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
            // dacDescription
            // 
            resources.ApplyResources(this.dacDescription, "dacDescription");
            this.dacDescription.BackColor = System.Drawing.Color.DarkBlue;
            this.dacDescription.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.dacDescription.ForeColor = System.Drawing.Color.WhiteSmoke;
            this.dacDescription.Name = "dacDescription";
            // 
            // mdtEditor
            // 
            resources.ApplyResources(this, "$this");
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(10)))), ((int)(((byte)(10)))), ((int)(((byte)(20)))));
            this.BackgroundImage = global::mdteditor.Properties.Resources.mdtracker_logo2;
            this.Controls.Add(this.dacDescription);
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
            this.tcInstruments.ResumeLayout(false);
            panelOpenSave.ResumeLayout(false);
            panelOpenSave.PerformLayout();
            pnAutoRange.ResumeLayout(false);
            pnAutoRange.PerformLayout();
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
        private System.Windows.Forms.Panel pnSamplesPool;
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
        private System.Windows.Forms.TextBox txtAssignStart_SRM;
        private System.Windows.Forms.TextBox txtAssignEnd_SRM;
        private System.Windows.Forms.TextBox txtAssignEnd_ROM;
        private System.Windows.Forms.TextBox txtAssignStart_ROM;
        private System.Windows.Forms.Panel pnPresetsPool;
        private System.Windows.Forms.Button btnAddPresets;
        private System.Windows.Forms.TabControl tcInstruments;
        private System.Windows.Forms.TabPage tbInstSRM;
        private System.Windows.Forms.TabPage tbInstROM;
        private System.Windows.Forms.TextBox txtPresetTarget;
        private System.Windows.Forms.TextBox txtPresetsSource;
        private System.Windows.Forms.Button btnAssignPreset;
        private System.Windows.Forms.Label dacDescription;
        private System.Windows.Forms.Label bankAddress;
        private System.Windows.Forms.TextBox SampleBankAddress;
    }
}

