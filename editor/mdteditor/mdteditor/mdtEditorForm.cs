using System;
using System.Collections.Generic;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace mdteditor
{
    public partial class mdtEditor : Form
    {
        private const int ROM_SAMPLE_BANK = 0x0002F400; // symbol.txt sample_bank_1; check when md.tracker code is changed
        private const int ROM_SAMPLE_BANK_SIZE = 3399000;

        private const int SRM_FILE_SIZE = 524288;
        private const uint SRM_SAMPLE_DATA = 0x0726B;
        private const uint SRM_SAMPLE_PAN = 0x6B0AB;
        private const uint SRM_SAMPLE_RATE = 0x6F230;

        private const uint SRM_PRESET_DATA = 0x00002;
        private const uint SRM_PRESET_NAME = 81;

        private const byte NOTES_COUNT = 96;
        private const int NOTES_TOTAL = 96 * 4;

        private const byte SOUND_PAN_LEFT      = 0x80;
        private const byte SOUND_PAN_RIGHT     = 0x40;
        private const byte SOUND_PAN_CENTER    = 0xC0;

        private const byte VGI_SIZE = 43;
        private const byte TFI_SIZE = 42;

/*
TFI (TFM Maker format)

TFM Maker instruments have the ".tfi" file extension and consist of 42 bytes. The first two bytes are:
TFI format initial bytes Size	Description	Range
1 byte	Algorithm	0 to 7
1 byte	Feedback	0 to 7
    1 byte	PAN FMS AMS ($B4+ from the YM2612) .vgi

The following 40 bytes are for the operators, with each group of 10 bytes being an operator. Operators come in S1, S3, S2, S4 order.
These bytes are as follows (repeat for each operator):
Data for each operator Size	Description	Range
1 byte	Multiplier	    0 to 15
1 byte	Detune	        0 to 6
1 byte	Total level	    0 to 127
1 byte	Rate scaling	0 to 3
1 byte	Attack rate	    0 to 31
1 byte	Decay rate	    0 to 31
1 byte	Sustain rate	0 to 31
1 byte	Release rate	0 to 15
1 byte	Sustain level	0 to 15
1 byte	SSG-EG	        0 to 15

All values use the same format as the corresponding YM2612 registers, except for detune.
        In order to get the detune, substract 3 (to put it in the -3 to +3 range),
        then convert that to the format the YM2612 wants.

VGM Maker instruments have the ".vgi" file extension and consist of 43 bytes.
It's almost identical to TFI files, except that an extra byte follows feedback,
which includes FMS and AMS (in the same format as register $B4+ from the YM2612). 
*/
/*  MD.Tracker srm
#define INSTRUMENT_DATA     0x00002 // 89 * 256 bytes
#define GLOBAL_LFO          0x05902 // INSTRUMENT_DATA + 5900h; 1 byte
#define FILE_CHECKER        0x05903 // DEAD. To check if SRAM file exists; 2 bytes
#define PATTERN_MATRIX      0x05905 // MAX_MATRIX_ROWS * 13 * 2 bytes
#define TEMPO               0x07269 // PATTERN_MATRIX + 1964h; 2 bytes
#define SAMPLE_DATA         0x0726B // 4 * 96 * SAMPLE_DATA_SIZE(7) bytes (3byte start + 3byte end + 1byte loop); 1byte rate is missing
#define PATTERN_DATA        0x07CEB // SAMPLE_DATA + A80h;
#define PATTERN_COLOR       0x6A06B // PATTERN_DATA + PATTERN_SIZE * (MAX_PATTERN + 1);
#define MATRIX_TRANSPOSE    0x6A3EC // matrix slot transpose (250*13)
#define MUTE_CHANNEL        0x6B09E // store disabled matrix channels (13)
#define SAMPLE_PAN          0x6B0AB // default sample pan (4 * 96)
//#define SONG_TRANSPOSE      0x6B22B // 1 byte
#define SEQ_VOL_START       0x6B230 // 32 steps vol seq start
#define SEQ_ARP_START       0x6D230 // SEQ_VOL_START + 2000; 32 steps arp seq start
#define SAMPLE_RATE         0x6F230 // SEQ_ARP_START + 2000; default sample rate (4 * 96)
//0x71230
// ...
//0x80000 // eof
*/
/*
#define INST_ALG 0 // 1 byte ..
#define INST_FMS 1
#define INST_AMS 2
#define INST_PAN 3
#define INST_FB 4

#define INST_TL1 5
#define INST_TL2 6
#define INST_TL3 7
#define INST_TL4 8

#define INST_RS1 9
#define INST_RS2 10
#define INST_RS3 11
#define INST_RS4 12

#define INST_MUL1 13
#define INST_MUL2 14
#define INST_MUL3 15
#define INST_MUL4 16

#define INST_DT1 17
#define INST_DT2 18
#define INST_DT3 19
#define INST_DT4 20

#define INST_AR1 21
#define INST_AR2 22
#define INST_AR3 23
#define INST_AR4 24

#define INST_D1R1 25
#define INST_D1R2 26
#define INST_D1R3 27
#define INST_D1R4 28

#define INST_D1L1 29
#define INST_D1L2 30
#define INST_D1L3 31
#define INST_D1L4 32

#define INST_D2R1 33
#define INST_D2R2 34
#define INST_D2R3 35
#define INST_D2R4 36

#define INST_RR1 37
#define INST_RR2 38
#define INST_RR3 39
#define INST_RR4 40

#define INST_AM1 41
#define INST_AM2 42
#define INST_AM3 43
#define INST_AM4 44

#define INST_SSGEG1 45
#define INST_SSGEG2 46
#define INST_SSGEG3 47
#define INST_SSGEG4 48

// 49..80 (32 bytes) unused!
//#define INST_VOL_TICK_01 49
//#define INST_VOL_TICK_16 64
//#define INST_ARP_TICK_01 65
//#define INST_ARP_TICK_16 80

#define INST_NAME_1 81
#define INST_NAME_8 88
*/

        private byte[] srmFile = new byte[SRM_FILE_SIZE];
        private List<Button> lsSamplesBank_Sync = new List<Button>();
        private List<TextBox> lsSamplesBank_Start, lsSamplesBank_End, lsSamplesBank_ID;
        private List<ComboBox> lsSamplesBank_Pan, lsSamplesBank_Rate;
        private List<CheckBox> lsSamplesBank_Loop;
        private Dictionary<string, int> dicSamplesPool_ID = new Dictionary<string, int>();
        private List<long> lsSamplesPool_Start, lsSamplesPool_End;
        private Dictionary<int, byte[]> dicSamplesPool_File = new Dictionary<int, byte[]>();
        private Dictionary<int, byte[]> dicPresetsPool_File = new Dictionary<int, byte[]>();
        private List<int> lsSamplesPool_Size;

        private TabPage[] tcBanks = new TabPage[4];

        private int samplesCount = 0, presetsCount = 0;
        private long sampleStart;
        private long sampleEnd = -1;
        private List<int> lsPresetTypes = new List<int>();

        // GUI
        private static readonly List<string> lsNoteNames = new List<string> { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
        private const int GUI_SAMPLE_SETTINGS_FIELD_HEIGHT = 25;

        private const int WM_HSCROLL = 0x114;
        private const int WM_VSCROLL = 0x115;

        public Button[] btnSampleSync = new Button[NOTES_TOTAL];
        public TextBox[] txtSampleStart = new TextBox[NOTES_TOTAL];
        public TextBox[] txtSampleEnd = new TextBox[NOTES_TOTAL];
        public ComboBox[] cbxSamplePan = new ComboBox[NOTES_TOTAL];
        public CheckBox[] chkSampleLoop = new CheckBox[NOTES_TOTAL];
        public ComboBox[] cbxSampleRate = new ComboBox[NOTES_TOTAL];
        public TextBox[] txtSampleID = new TextBox[NOTES_TOTAL];

        //public Label[] presetID = new Label[512];
        public Label[] presetName = new Label[512]; // 0th instrument is empty, 255 user, 256 midi

        /*static void SetDoubleBuffer(Control ctl, bool doubleBuffered)
        {
            try
            {
                typeof(Control).InvokeMember("DoubleBuffered",
                BindingFlags.NonPublic | BindingFlags.Instance | BindingFlags.SetProperty,
                null, ctl, new object[] { doubleBuffered });
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }*/

        /*protected override void WndProc(ref Message m)
        {
            if ((m.Msg == WM_HSCROLL || m.Msg == WM_VSCROLL)
            && (((int)m.WParam & 0xFFFF) == 5))
            {
                // Change SB_THUMBTRACK to SB_THUMBPOSITION
                m.WParam = (IntPtr)(((int)m.WParam & ~0xFFFF) | 4);
            }
            base.WndProc(ref m);
        }*/

        /*protected override void WndProc(ref Message m)
        {
            Debug.WriteLine($"msg: {m.Msg}, wparam:{m.WParam}, lparam:{m.LParam}, lparam:{m.HWnd}");
            base.WndProc(ref m);
        }*/

        /*protected override CreateParams CreateParams
        {
            get
            {
                var cp = base.CreateParams;
                cp.ExStyle |= 0x02000000;    // Turn on WS_EX_COMPOSITED
                return cp;
            }
        }*/

        public mdtEditor()
        {
            InitializeComponent();

            tabControl_SampleBanks.SelectedIndexChanged += new EventHandler(TabChanged);

            tcBanks[0] = pageSamplesBank1;
            tcBanks[1] = pageSamplesBank2;
            tcBanks[2] = pageSamplesBank3;
            tcBanks[3] = pageSamplesBank4;

            /*for (int l = 0; l < 4; l++)
            {
                if (tabControl_SampleBanks.SelectedIndex == l) tcBanks[l].ResumeLayout(); else tcBanks[l].SuspendLayout();
            }*/
        }

        private void TabChanged(object sender, EventArgs e)
        {
            TabControl tc = sender as TabControl;
            foreach (TabPage tp in tcBanks) tp.SuspendLayout();
            tcBanks[tc.SelectedIndex].ResumeLayout(true);
        }

        // generated
        private void ButtonClicked(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            int id = dicSamplesPool_ID[btn.Name];
            if (lsSamplesBank_ID[id].Text != null)
            {
                int.TryParse(lsSamplesBank_ID[id].Text, out int syncID);

                if (syncID >= 0 && syncID < lsSamplesPool_Start.Count)
                {
                    lsSamplesBank_Start[id].Text = lsSamplesPool_Start[syncID].ToString();
                    lsSamplesBank_End[id].Text = lsSamplesPool_End[syncID].ToString();
                }
            }
        }

        private void GenerateBankForm()
        {
            int counter = 0;

            for (int bank = 0; bank < 4; bank++)
            {
                for (int octave = 0; octave < 8; octave++)
                {
                    for (int note = 0; note < 12; note++)
                    {
                        int y;
                        y = 5 + (note * GUI_SAMPLE_SETTINGS_FIELD_HEIGHT) + (12 * GUI_SAMPLE_SETTINGS_FIELD_HEIGHT * octave);

                        string name;
                        //name = "_Bank" + bank.ToString() + "_Octave" + octave.ToString() + "_Note" + note.ToString();

                        StringBuilder sb;
                        sb = new StringBuilder();
                        sb.Append("_Bank"); sb.Append(bank.ToString());
                        sb.Append("_Octave"); sb.Append(octave.ToString());
                        sb.Append("_Note"); sb.Append(note.ToString());
                        name = sb.ToString();

                        btnSampleSync[counter] = new Button
                        {
                            Location = new Point(5, y),
                            Name = string.Concat("btnSync", name),
                            Width = 70,
                            TextAlign = ContentAlignment.MiddleRight,
                            Text = string.Concat(counter.ToString(), ": ", lsNoteNames[note], octave.ToString()),
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.Gold,
                            FlatStyle = FlatStyle.Flat
                        };

                        txtSampleStart[counter] = new TextBox
                        {
                            Location = new Point(80, y),
                            Name = string.Concat("txtStart", name),
                            Width = 100,
                            Text = "",
                            TextAlign = HorizontalAlignment.Right,
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.PaleGoldenrod,
                            BorderStyle = BorderStyle.FixedSingle
                        };

                        txtSampleEnd[counter] = new TextBox
                        {
                            Location = new Point(190, y),
                            Name = string.Concat("txtEnd", name),
                            Width = 100,
                            Text = "",
                            TextAlign = HorizontalAlignment.Right,
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.PaleGoldenrod,
                            BorderStyle = BorderStyle.FixedSingle
                        };

                        txtSampleID[counter] = new TextBox
                        {
                            Location = new Point(300, y),
                            Name = string.Concat("txtID", name),
                            Width = 40,
                            Text = "0",
                            TextAlign = HorizontalAlignment.Right,
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.AliceBlue,
                            BorderStyle = BorderStyle.FixedSingle
                        };

                        cbxSamplePan[counter] = new ComboBox
                        {
                            Location = new Point(350, y),
                            Name = string.Concat("cbxPan", name),
                            Width = 70,
                            Text = "",
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.LightGoldenrodYellow,
                            FlatStyle = FlatStyle.Flat,
                        };
                        cbxSamplePan[counter].Items.AddRange( new string[] { "CENTER", "LEFT", "RIGHT" } );
                        cbxSamplePan[counter].SelectedIndex = 0;

                        cbxSampleRate[counter] = new ComboBox
                        {
                            Location = new Point(430, y),
                            Name = string.Concat("cbxPan", name),
                            Width = 60,
                            Text = "",
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.LightGoldenrodYellow,
                            FlatStyle = FlatStyle.Flat
                        };
                        cbxSampleRate[counter].Items.AddRange( new string[] { "32000", "22050", "16000", "13400", "11025", "8000" } );
                        cbxSampleRate[counter].SelectedIndex = 0;

                        chkSampleLoop[counter] = new CheckBox
                        {
                            Location = new Point(500, y),
                            Name = string.Concat("chkLoop", name),
                            Width = 60,
                            Text = "LOOP",
                            Checked = false,
                            BackColor = Color.FromArgb(255, 10, 10, 15),
                            ForeColor = Color.PaleGoldenrod,
                            FlatStyle = FlatStyle.Flat
                        };

                        /*lsSamplesBank_Sync.Add(btnSampleSync[counter]);
                        tcBanks[bank].Controls.Add(btnSampleSync[counter]);
                        lsSamplesBank_Start.Add(txtSampleStart[counter]);
                        tcBanks[bank].Controls.Add(txtSampleStart[counter]);
                        lsSamplesBank_End.Add(txtSampleEnd[counter]);
                        tcBanks[bank].Controls.Add(txtSampleEnd[counter]);
                        lsSamplesBank_ID.Add(txtSampleID[counter]);
                        tcBanks[bank].Controls.Add(txtSampleID[counter]);
                        lsSamplesBank_Pan.Add(cbxSamplePan[counter]);
                        tcBanks[bank].Controls.Add(cbxSamplePan[counter]);
                        lsSamplesBank_Rate.Add(cbxSampleRate[counter]);
                        tcBanks[bank].Controls.Add(cbxSampleRate[counter]);
                        lsSamplesBank_Loop.Add(chkSampleLoop[counter]);
                        tcBanks[bank].Controls.Add(chkSampleLoop[counter]);*/

                        counter++;
                    }
                }             
            }
        
            //faster loading
            Button[] btn1 = new Button[NOTES_COUNT];
            Button[] btn2 = new Button[NOTES_COUNT];
            Button[] btn3 = new Button[NOTES_COUNT];
            Button[] btn4 = new Button[NOTES_COUNT];
            TextBox[] txt1 = new TextBox[NOTES_COUNT];
            TextBox[] txt2 = new TextBox[NOTES_COUNT];
            TextBox[] txt3 = new TextBox[NOTES_COUNT];
            TextBox[] txt4 = new TextBox[NOTES_COUNT];
            ComboBox[] cbx1 = new ComboBox[NOTES_COUNT];
            ComboBox[] cbx2 = new ComboBox[NOTES_COUNT];
            ComboBox[] cbx3 = new ComboBox[NOTES_COUNT];
            ComboBox[] cbx4 = new ComboBox[NOTES_COUNT];
            CheckBox[] chk1 = new CheckBox[NOTES_COUNT];
            CheckBox[] chk2 = new CheckBox[NOTES_COUNT];
            CheckBox[] chk3 = new CheckBox[NOTES_COUNT];
            CheckBox[] chk4 = new CheckBox[NOTES_COUNT];

            Array.Copy(btnSampleSync, 96 * 0, btn1, 0, NOTES_COUNT);
            Array.Copy(btnSampleSync, 96 * 1, btn2, 0, NOTES_COUNT);
            Array.Copy(btnSampleSync, 96 * 2, btn3, 0, NOTES_COUNT);
            Array.Copy(btnSampleSync, 96 * 3, btn4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(btn1);
            tcBanks[1].Controls.AddRange(btn2);
            tcBanks[2].Controls.AddRange(btn3);
            tcBanks[3].Controls.AddRange(btn4);

            Array.Copy(txtSampleStart, 96 * 0, txt1, 0, NOTES_COUNT);
            Array.Copy(txtSampleStart, 96 * 1, txt2, 0, NOTES_COUNT);
            Array.Copy(txtSampleStart, 96 * 2, txt3, 0, NOTES_COUNT);
            Array.Copy(txtSampleStart, 96 * 3, txt4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(txt1);
            tcBanks[1].Controls.AddRange(txt2);
            tcBanks[2].Controls.AddRange(txt3);
            tcBanks[3].Controls.AddRange(txt4);

            Array.Copy(txtSampleEnd, 96 * 0, txt1, 0, NOTES_COUNT);
            Array.Copy(txtSampleEnd, 96 * 1, txt2, 0, NOTES_COUNT);
            Array.Copy(txtSampleEnd, 96 * 2, txt3, 0, NOTES_COUNT);
            Array.Copy(txtSampleEnd, 96 * 3, txt4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(txt1);
            tcBanks[1].Controls.AddRange(txt2);
            tcBanks[2].Controls.AddRange(txt3);
            tcBanks[3].Controls.AddRange(txt4);

            Array.Copy(txtSampleID, 96 * 0, txt1, 0, NOTES_COUNT);
            Array.Copy(txtSampleID, 96 * 1, txt2, 0, NOTES_COUNT);
            Array.Copy(txtSampleID, 96 * 2, txt3, 0, NOTES_COUNT);
            Array.Copy(txtSampleID, 96 * 3, txt4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(txt1);
            tcBanks[1].Controls.AddRange(txt2);
            tcBanks[2].Controls.AddRange(txt3);
            tcBanks[3].Controls.AddRange(txt4);

            Array.Copy(cbxSamplePan, 96 * 0, cbx1, 0, NOTES_COUNT);
            Array.Copy(cbxSamplePan, 96 * 1, cbx2, 0, NOTES_COUNT);
            Array.Copy(cbxSamplePan, 96 * 2, cbx3, 0, NOTES_COUNT);
            Array.Copy(cbxSamplePan, 96 * 3, cbx4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(cbx1);
            tcBanks[1].Controls.AddRange(cbx2);
            tcBanks[2].Controls.AddRange(cbx3);
            tcBanks[3].Controls.AddRange(cbx4);

            Array.Copy(cbxSampleRate, 96 * 0, cbx1, 0, NOTES_COUNT);
            Array.Copy(cbxSampleRate, 96 * 1, cbx2, 0, NOTES_COUNT);
            Array.Copy(cbxSampleRate, 96 * 2, cbx3, 0, NOTES_COUNT);
            Array.Copy(cbxSampleRate, 96 * 3, cbx4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(cbx1);
            tcBanks[1].Controls.AddRange(cbx2);
            tcBanks[2].Controls.AddRange(cbx3);
            tcBanks[3].Controls.AddRange(cbx4);

            Array.Copy(chkSampleLoop, 96 * 0, chk1, 0, NOTES_COUNT);
            Array.Copy(chkSampleLoop, 96 * 1, chk2, 0, NOTES_COUNT);
            Array.Copy(chkSampleLoop, 96 * 2, chk3, 0, NOTES_COUNT);
            Array.Copy(chkSampleLoop, 96 * 3, chk4, 0, NOTES_COUNT);
            tcBanks[0].Controls.AddRange(chk1);
            tcBanks[1].Controls.AddRange(chk2);
            tcBanks[2].Controls.AddRange(chk3);
            tcBanks[3].Controls.AddRange(chk4);

            lsSamplesBank_Sync.AddRange(btnSampleSync);
            lsSamplesBank_Start.AddRange(txtSampleStart);
            lsSamplesBank_End.AddRange(txtSampleEnd);
            lsSamplesBank_ID.AddRange(txtSampleID);
            lsSamplesBank_Pan.AddRange(cbxSamplePan);
            lsSamplesBank_Rate.AddRange(cbxSampleRate);
            lsSamplesBank_Loop.AddRange(chkSampleLoop);

            for (int i = 0; i < (NOTES_COUNT * 4); i++)
            {
                dicSamplesPool_ID.Add(lsSamplesBank_Sync[i].Name, i);
                lsSamplesBank_Sync[i].Click += new EventHandler(ButtonClicked);
            }

            tcBanks[0].Refresh();
            tcBanks[1].Refresh();
            tcBanks[2].Refresh();
            tcBanks[3].Refresh();
        }
        private void GeneratePresetsForm()
        {
            int y;
            string name = "--------";

            // srm
            for (int i = 0; i < 512; i++)
            {
                if (i < 256)
                {
                    y = 5 + i * GUI_SAMPLE_SETTINGS_FIELD_HEIGHT;
                }
                else
                {
                    name = "-------------";
                    y = 5 + (i - 256) * GUI_SAMPLE_SETTINGS_FIELD_HEIGHT;
                }

                Label lbID = new Label
                {
                    Location = new Point(5, y),
                    Name = string.Concat("lbPreset_ID_", i.ToString()),
                    Width = 70,
                    TextAlign = ContentAlignment.MiddleRight,
                    Text = i.ToString(),
                    BackColor = Color.FromArgb(255, 10, 10, 15),
                    ForeColor = Color.Gold,
                    FlatStyle = FlatStyle.Flat
                };

                presetName[i] = new Label
                {
                    Location = new Point(80, y),
                    Name = string.Concat("lbPreset_Name_", i.ToString()),
                    Width = 100,
                    TextAlign = ContentAlignment.MiddleRight,
                    Text = name,
                    BackColor = Color.FromArgb(255, 10, 10, 15),
                    ForeColor = Color.Gold,
                    FlatStyle = FlatStyle.Flat
                };

                if (i < 256)
                {
                    tbInstSRM.Controls.Add(lbID);
                    tbInstSRM.Controls.Add(presetName[i]);
                }
                else
                {
                    tbInstROM.Controls.Add(lbID);
                    tbInstROM.Controls.Add(presetName[i]);
                }
            }
        }

        // init bank panels
        private void Form1_Load(object sender, EventArgs e)
        {
            lsSamplesBank_Start = new List<TextBox>();
            lsSamplesBank_End = new List<TextBox>();
            lsSamplesBank_ID = new List<TextBox>();
            lsSamplesPool_Start = new List<long>();
            lsSamplesPool_End = new List<long>();
            lsSamplesPool_Size = new List<int>();
            lsSamplesBank_Pan = new List<ComboBox>();
            lsSamplesBank_Rate = new List<ComboBox>();
            lsSamplesBank_Loop = new List<CheckBox>();

            /*tcBanks[0] = pageSamplesBank1;
            tcBanks[1] = pageSamplesBank2;
            tcBanks[2] = pageSamplesBank3;
            tcBanks[3] = pageSamplesBank4;*/

            //CheckForIllegalCrossThreadCalls = false;
            /*DoubleBuffered = true;
            SetDoubleBuffer(tabControl_SampleBanks, true);
            SetDoubleBuffer(pageSamplesBank1, true);
            SetDoubleBuffer(pageSamplesBank2, true);
            SetDoubleBuffer(pageSamplesBank3, true);
            SetDoubleBuffer(pageSamplesBank4, true);*/

            /*Task.Run
            (
                () =>
                {*/
            GenerateBankForm();
            GeneratePresetsForm();
                /*}
            );*/
        }

        // assign all available samples
        private void btnAutoRange_Click(object sender, EventArgs e)
        {
            //int activeBank;
            //activeBank = tabControl_SampleBanks.SelectedIndex;

            int.TryParse(txtAssignStart_ROM.Text, out int start);
            int.TryParse(txtAssignEnd_ROM.Text, out int end);
            int.TryParse(txtAssignStart_SRM.Text, out int startKey);
            int.TryParse(txtAssignEnd_SRM.Text, out int endKey);

            if (startKey < 0 || endKey < 0 || start < 0 || end < 0) return;

            for (int id = start; id <= end; id++)
            {
                int keyId = id + startKey;
                if (id >= NOTES_TOTAL || id >= samplesCount || keyId > endKey) break;
                lsSamplesBank_ID[keyId].Text = id.ToString();
                lsSamplesBank_Sync[keyId].PerformClick();
            }
        }

        private void btnAutoPan_Click(object sender, EventArgs e)
        {
            int.TryParse(txtAssignStart_SRM.Text, out int startKey);
            int.TryParse(txtAssignEnd_SRM.Text, out int endKey);
            if (startKey < 0 || endKey < 0) return;
            for (int id = startKey; id <= endKey; id++)
            {
                if (id >= NOTES_TOTAL) break;
                lsSamplesBank_Pan[id ].SelectedItem = lsSamplesBank_Pan[startKey].SelectedItem;
            }
        }

        private void btnAutoRate_Click(object sender, EventArgs e)
        {
            int.TryParse(txtAssignStart_SRM.Text, out int startKey);
            int.TryParse(txtAssignEnd_SRM.Text, out int endKey);
            if (startKey < 0 || endKey < 0) return;
            for (int id = startKey; id <= endKey; id++)
            {
                if (id >= NOTES_TOTAL) break;
                lsSamplesBank_Rate[id].SelectedItem = lsSamplesBank_Rate[startKey].SelectedItem;
            }
        }

        private void btnAutoLoop_Click(object sender, EventArgs e)
        {
            int.TryParse(txtAssignStart_SRM.Text, out int startKey);
            int.TryParse(txtAssignEnd_SRM.Text, out int endKey);
            if (startKey < 0 || endKey < 0) return;
            for (int id = startKey; id <= endKey; id++)
            {
                if (id >= NOTES_TOTAL) break;
                lsSamplesBank_Loop[id].Checked = lsSamplesBank_Loop[startKey].Checked;
            }
        }

        // mdt sram. byteswapped
        private uint SRAMW_readByte(uint offset)
        {
            if (offset < 2) return 0;

            //if ((offset % 2) == 0) offset++; else offset--;
            if ((offset & 1) == 1) offset -= 2;
            return offset;
        }

        private void btnAddPresets_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog
            {
                Filter = string.Concat
                (
                    "VGM Maker (*.vgi)|*.vgi|",
                    "TFM Maker (*.tfi)|*.tfi|",
                    "Any file (*.*)|*.*"
                ),
                Multiselect = true,
                Title = "Add Presets",
                FilterIndex = 1
            };

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                for (int i = 0; i < ofd.SafeFileNames.Length; i++)
                {
                    int id = i + presetsCount;
                    int y = 5 + 25 * id;

                    Label lbID = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_PresetID_", id.ToString()),
                        Text = id.ToString(),
                        Width = 40,
                        Location = new Point(5, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleCenter,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnPresetsPool.Controls.Add(lbID);

                    Label lbFileName = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_PresetName_", id.ToString()),
                        Text = ofd.SafeFileNames[i],
                        Width = 450,
                        Location = new Point(50, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleLeft,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnPresetsPool.Controls.Add(lbFileName);

                    FileInfo fi = new FileInfo(ofd.FileNames[i]);

                    switch(fi.Extension)
                    {
                        case "vgi": lsPresetTypes.Add(0); break;
                        case "tfi": lsPresetTypes.Add(1); break;
                        default: lsPresetTypes.Add(0);  break;
                    }

                    FileStream fs = new FileStream(ofd.FileNames[i], FileMode.Open, FileAccess.Read);
                    BinaryReader br = new BinaryReader(fs);

                    byte[] file = new byte[fs.Length];
                    fs.Read(file, 0, file.Length);

                    dicPresetsPool_File.Add(id, file);

                    br.Close(); fs.Close();
                }
                presetsCount += ofd.SafeFileNames.Length;
            }
        }

        // mdt sram. byteswapped
        /*private uint SRAMW_writeByte(uint offset)
        {
            if ((offset & 1) == 1) offset -= 2;
            //if ((offset % 2) == 0) offset++; else offset--;
            return offset;
        }*/

        // open ROM
        private void btnOpenROM_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofdROM = new OpenFileDialog();
            ofdROM.Filter = "MD.Tracker ROM (*.bin)|*.bin|Any File (*.*)|*.*";
            ofdROM.Multiselect = false;
            ofdROM.Title = "Open ROM";
            if (ofdROM.ShowDialog() == DialogResult.OK)
            {
                txtRomName.Text = ofdROM.FileName;
                FileStream fs = new FileStream(ofdROM.FileName, FileMode.Open, FileAccess.Read);
                BinaryReader br = new BinaryReader(fs);

                // load presets from rom

                br.Close(); fs.Close();
            }
        }

        // save ROM
        private void btnSaveRom_Click(object sender, EventArgs e)
        {
            // save sample bank data
            byte[] sampleData = new byte[ROM_SAMPLE_BANK_SIZE];
            int index = 0;
            for (int id = 0; id < samplesCount; id++)
            {
                dicSamplesPool_File.TryGetValue(id, out byte[] sample);

                if (chkReplaceFF.Checked)
                {
                    for(int i = 0; i < sample.Length; i++)
                    {
                        if (sample[i] == 0xFF) sample[i] = 0x00;
                    }
                }

                sample.CopyTo(sampleData, index);
                index += sample.Length;
            }

            if (txtRomName.Text != "")
            {
                FileInfo fin = new FileInfo(txtRomName.Text);
                if (fin.Exists)
                {
                    FileStream fs = new FileStream(fin.FullName, FileMode.Open, FileAccess.Write);
                    BinaryWriter bw = new BinaryWriter(fs);

                    bw.Seek(ROM_SAMPLE_BANK, SeekOrigin.Begin);
                    bw.Write(sampleData, 0, ROM_SAMPLE_BANK_SIZE);

                    bw.Flush(); bw.Close(); fs.Close();
                }
            }

            // save presets

        }
        
        // open SRM
        private void btnOpenSRM_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter =
                "MD.Tracker Save (*.sram)|*.sram|" +
                "MD.Tracker Save (*.srm)|*.srm|" +
                "Any File (*.*)|*.*";

            ofd.Multiselect = false;
            ofd.Title = "Open SRM";
            if (ofd.ShowDialog() == DialogResult.OK)
            {
                txtSrmName.Text = ofd.FileName;
                FileStream fs = new FileStream(ofd.FileName, FileMode.Open, FileAccess.Read);
                BinaryReader br = new BinaryReader(fs);

                br.Read(srmFile, 0, SRM_FILE_SIZE);

                // load sample settings
                for (int i = 0; i < NOTES_TOTAL; i++)
                {
                    uint addr = SRM_SAMPLE_DATA + ((uint)i * 7); // 0x0726B
                    uint addrPan = (uint)(SRM_SAMPLE_PAN + i);
                    uint addrRate = (uint)(SRM_SAMPLE_RATE + i);

                    uint st1 = SRAMW_readByte(addr + 0);
                    uint st2 = SRAMW_readByte(addr + 1);
                    uint st3 = SRAMW_readByte(addr + 2);
                    uint ed1 = SRAMW_readByte(addr + 3);
                    uint ed2 = SRAMW_readByte(addr + 4);
                    uint ed3 = SRAMW_readByte(addr + 5);
                    uint loop = SRAMW_readByte(addr + 6);
                    uint pan = SRAMW_readByte(addrPan);
                    uint rate = SRAMW_readByte(addrRate);
                    uint val;

                    // range
                    val = ((uint)srmFile[st1] << 16) | (uint)(srmFile[st2] << 8) | (srmFile[st3]);
                    lsSamplesBank_Start[i].Text = val.ToString();

                    val = ((uint)srmFile[ed1] << 16) | ((uint)srmFile[ed2] << 8) | (srmFile[ed3]);
                    lsSamplesBank_End[i].Text = val.ToString();

                    // pan
                    val = srmFile[pan];
                    switch (val)
                    {
                        case SOUND_PAN_LEFT: lsSamplesBank_Pan[i].SelectedIndex = 1; break;
                        case SOUND_PAN_RIGHT: lsSamplesBank_Pan[i].SelectedIndex = 2; break;
                        default: lsSamplesBank_Pan[i].SelectedIndex = 0; break;
                    }

                    // rate
                    val = srmFile[rate];
                    if (val <= 5)
                        lsSamplesBank_Rate[i].SelectedIndex = (int)val;

                    // loop
                    val = srmFile[loop];
                    if (val <= 1)
                        lsSamplesBank_Loop[i].Checked = Convert.ToBoolean(val);
                }

                br.Close(); fs.Close();
            }
        }

        // save SRM
        private void btnSaveSrm_Click(object sender, EventArgs e)
        {
            if (txtSrmName.Text != "")
            {
                FileInfo fin = new FileInfo(txtSrmName.Text);
                if (fin.Exists)
                {
                    FileStream fs = new FileStream(fin.FullName, FileMode.Open, FileAccess.Write);
                    BinaryWriter bw = new BinaryWriter(fs);

                    // load sample settings
                    for (int i = 0; i < NOTES_TOTAL; i++)
                    {
                        uint addr = SRM_SAMPLE_DATA + ((uint)i * 7); // 0x0726B
                        uint addrPan = (uint)(SRM_SAMPLE_PAN + i);
                        uint addrRate = (uint)(SRM_SAMPLE_RATE + i);

                        uint st1 = SRAMW_readByte(addr + 0);
                        uint st2 = SRAMW_readByte(addr + 1);
                        uint st3 = SRAMW_readByte(addr + 2);
                        uint ed1 = SRAMW_readByte(addr + 3);
                        uint ed2 = SRAMW_readByte(addr + 4);
                        uint ed3 = SRAMW_readByte(addr + 5);
                        uint loop = SRAMW_readByte(addr + 6);
                        uint pan = SRAMW_readByte(addrPan);
                        uint rate = SRAMW_readByte(addrRate);
                        uint val;

                        // range
                        val = Convert.ToUInt32(lsSamplesBank_Start[i].Text);
                        srmFile[st1] = Convert.ToByte(val >> 16);
                        srmFile[st2] = Convert.ToByte((val >> 8) & 0xFF);
                        srmFile[st3] = Convert.ToByte(val & 0xFF);


                        val = Convert.ToUInt32(lsSamplesBank_End[i].Text);
                        srmFile[ed1] = Convert.ToByte(val >> 16);
                        srmFile[ed2] = Convert.ToByte((val >> 8) & 0xFF);
                        srmFile[ed3] = Convert.ToByte(val & 0xFF);

                        // pan
                        val = (byte)lsSamplesBank_Pan[i].SelectedIndex;
                        switch (val)
                        {
                            case 1: srmFile[pan] = SOUND_PAN_LEFT; break;
                            case 2: srmFile[pan] = SOUND_PAN_RIGHT; break;
                            default: srmFile[pan] = SOUND_PAN_CENTER; break;
                        }

                        // rate
                        srmFile[rate] = (byte)lsSamplesBank_Rate[i].SelectedIndex;

                        // loop
                        srmFile[loop] = Convert.ToByte(lsSamplesBank_Loop[i].Checked);
                    }

                    bw.Write(srmFile, 0, SRM_FILE_SIZE);
                    bw.Flush(); bw.Close(); fs.Close();
                }
            }
        }

        // samples pool
        private void btnAddSamples_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog
            {
                Filter = "Signed 8 bit raw (*.raw)|*.raw|Any file (*.*)|*.*",
                Multiselect = true,
                Title = "Add Samples",
                FilterIndex = 2
            };

            if (ofd.ShowDialog() == DialogResult.OK)
            {
                for (int i = 0; i < ofd.SafeFileNames.Length; i++)
                {
                    int id = i + samplesCount;
                    int y = 5 + 25 * id;

                    Label lbID = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_id_", id.ToString()),
                        Text = id.ToString(),
                        Width = 40,
                        Location = new Point(5, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleCenter,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnSamplesPool.Controls.Add(lbID);

                    Label lbFileName = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_name_",  id.ToString()),
                        Text = ofd.SafeFileNames[i],
                        Width = 300,
                        Location = new Point(50, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleLeft,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnSamplesPool.Controls.Add(lbFileName);

                    FileInfo fi = new FileInfo(ofd.FileNames[i]);

                    // samples must be 256 bytes aligned, so we extend if too short
                    long chunks = fi.Length / 256;
                    long remain = fi.Length - chunks * 256;
                    long alignedLength = fi.Length;

                    if (remain > 0) alignedLength += 256 - remain;

                    Console.WriteLine($"chunks: {chunks}");

                    sampleStart = sampleEnd + 1;
                    sampleEnd += alignedLength;

                    lsSamplesPool_Start.Add(sampleStart);
                    lsSamplesPool_End.Add(sampleEnd);

                    Label lbSampleStart = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_start_", id.ToString()),
                        Text = sampleStart.ToString(),
                        Width = 80,
                        Location = new Point(360, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleRight,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnSamplesPool.Controls.Add(lbSampleStart);

                    Label lbSampleEnd = new Label
                    {
                        Name = string.Concat(ofd.SafeFileNames[i], "_end_", id.ToString()),
                        Text = sampleEnd.ToString(),
                        Width = 80,
                        Location = new Point(450, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleRight,
                        ForeColor = Color.PaleGoldenrod
                    };
                    pnSamplesPool.Controls.Add(lbSampleEnd);

                    FileStream fs = new FileStream(ofd.FileNames[i], FileMode.Open, FileAccess.Read);
                    BinaryReader br = new BinaryReader(fs);

                    byte[] file = new byte[alignedLength];
                    fs.Read(file, 0, file.Length);

                    dicSamplesPool_File.Add(id, file);
                    lsSamplesPool_Size.Add(file.Length);

                    br.Close(); fs.Close();
                }
                txtBytesUsed.Text = sampleEnd.ToString();
                samplesCount += ofd.SafeFileNames.Length;
            }
        }
    }
}
