using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Diagnostics;
using System.Reflection;

namespace mdteditor
{
    public partial class mdtEditor : Form
    {
        private readonly static int ROM_SAMPLE_BANK = 0x0002F700;
        private readonly static int ROM_SAMPLE_BANK_SIZE = 3399000;

        private readonly static int SRM_FILE_SIZE = 524288;
        private readonly static uint SRM_SAMPLE_DATA = 0x0726B;
        private readonly static uint SRM_SAMPLE_PAN = 0x6B0AB;
        private readonly static uint SRM_SAMPLE_RATE = 0x6F230;

        private readonly static byte NOTES_COUNT = 96;
        private readonly static uint NOTES_TOTAL = 96 * 4;

        /*  MD.Tracker
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

        private byte[] srmFile = new byte[SRM_FILE_SIZE];
        private List<Button> lsSamplesBank_Sync = new List<Button>();
        private List<TextBox> lsSamplesBank_Start, lsSamplesBank_End, lsSamplesBank_ID;
        private List<ComboBox> lsSamplesBank_Pan, lsSamplesBank_Rate;
        private List<CheckBox> lsSamplesBank_Loop;
        private Dictionary<string, int> dicSamplesPool_ID = new Dictionary<string, int>();
        private List<long> lsSamplesPool_Start, lsSamplesPool_End;
        private Dictionary<int, byte[]> dicSamplesPool_File = new Dictionary<int, byte[]>();
        private List<int> lsSamplesPool_Size;

        private TabPage[] tcBanks = new TabPage[4];

        private int samplesCount = 0;
        private long sampleStart = -1, sampleEnd = 0;

        // GUI
        private static List<string> lsNoteNames = new List<string> { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
        private readonly static int GUI_SAMPLE_SETTINGS_FIELD_HEIGHT = 25;

        private const int WM_HSCROLL = 0x114;
        private const int WM_VSCROLL = 0x115;

        public Button[] btnSampleSync = new Button[NOTES_TOTAL];
        public TextBox[] txtSampleStart = new TextBox[NOTES_TOTAL];
        public TextBox[] txtSampleEnd = new TextBox[NOTES_TOTAL];
        public ComboBox[] cbxSamplePan = new ComboBox[NOTES_TOTAL];
        public CheckBox[] chkSampleLoop = new CheckBox[NOTES_TOTAL];
        public ComboBox[] cbxSampleRate = new ComboBox[NOTES_TOTAL];
        public TextBox[] txtSampleID = new TextBox[NOTES_TOTAL];

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
                            Name = "btnSync" + name,
                            Width = 40,
                            Text = lsNoteNames[note] + octave.ToString()
                        };

                        txtSampleStart[counter] = new TextBox
                        {
                            Location = new Point(50, y),
                            Name = "txtStart" + name,
                            Width = 100,
                            Text = "",
                            TextAlign = HorizontalAlignment.Right
                        };

                        txtSampleEnd[counter] = new TextBox
                        {
                            Location = new Point(160, y),
                            Name = "txtEnd" + name,
                            Width = 100,
                            Text = "",
                            TextAlign = HorizontalAlignment.Right
                        };

                        txtSampleID[counter] = new TextBox
                        {
                            Location = new Point(270, y),
                            Name = "txtID" + name,
                            Width = 40,
                            Text = "0",
                            TextAlign = HorizontalAlignment.Right
                        };

                        cbxSamplePan[counter] = new ComboBox
                        {
                            Location = new Point(320, y),
                            Name = "cbxPan" + name,
                            Width = 70,
                            Text = ""
                        };
                        cbxSamplePan[counter].Items.AddRange( new string[] { "MUTE", "RIGHT", "LEFT", "CENTER" } );
                        cbxSamplePan[counter].SelectedIndex = 3;

                        cbxSampleRate[counter] = new ComboBox
                        {
                            Location = new Point(400, y),
                            Name = "cbxPan" + name,
                            Width = 60,
                            Text = ""
                        };
                        cbxSampleRate[counter].Items.AddRange( new string[] { "32000", "22050", "16000", "13400", "11025", "8000" } );
                        cbxSampleRate[counter].SelectedIndex = 0;

                        chkSampleLoop[counter] = new CheckBox
                        {
                            Location = new Point(470, y),
                            Name = "chkLoop" + name,
                            Width = 60,
                            Text = "LOOP",
                            Checked = false
                        };

                        lsSamplesBank_Sync.Add(btnSampleSync[counter]);
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
                        tcBanks[bank].Controls.Add(chkSampleLoop[counter]);

                        counter++;
                    }
                }             
            }
        
            //faster loading
            /*Button[] btn1 = new Button[NOTES_COUNT];
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
            lsSamplesBank_Loop.AddRange(chkSampleLoop);*/

            for (int i = 0; i < (NOTES_COUNT * 4); i++)
            {
                dicSamplesPool_ID.Add(lsSamplesBank_Sync[i].Name, i);
                lsSamplesBank_Sync[i].Click += new EventHandler(ButtonClicked);
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
                /*}
            );*/
        }

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

        // assign all available samples
        private void btnAutoRange_Click(object sender, EventArgs e)
        {
            //int activeBank;
            //activeBank = tabControl_SampleBanks.SelectedIndex;
            for (int id = 0; id < samplesCount; id++)
            {
                if (id == NOTES_TOTAL) break;
                lsSamplesBank_ID[id].Text = id.ToString();
                lsSamplesBank_Sync[id].PerformClick();
            }
        }

        private void btnAutoPan_Click(object sender, EventArgs e)
        {
            int offset = tabControl_SampleBanks.SelectedIndex * NOTES_COUNT;
            for (int id = 0; id < NOTES_COUNT; id++)
            {
                lsSamplesBank_Pan[id + offset].SelectedItem = lsSamplesBank_Pan[offset].SelectedItem;
            }
        }

        private void btnAutoRate_Click(object sender, EventArgs e)
        {
            int offset = tabControl_SampleBanks.SelectedIndex * NOTES_COUNT;
            for (int id = 0; id < NOTES_COUNT; id++)
            {
                lsSamplesBank_Rate[id + offset].SelectedItem = lsSamplesBank_Rate[offset].SelectedItem;
            }
        }

        private void btnAutoLoop_Click(object sender, EventArgs e)
        {
            int offset = tabControl_SampleBanks.SelectedIndex * NOTES_COUNT;
            for (int id = 0; id < NOTES_COUNT; id++)
            {
                lsSamplesBank_Loop[id + offset].Checked = lsSamplesBank_Loop[offset].Checked;
            }
        }

        // mdt sram. byteswapped
        private uint SRAMW_readByte(uint offset)
        {
            //if ((offset % 2) == 0) offset -= 2;
            if ((offset % 2) == 0) offset++; else offset--;
            return offset;
        }

        // open SRM
        private void btnOpenSRM_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "MD.Tracker Save (*.srm)|*.srm|Any File (*.*)|*.*";
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
                    if (val <= 3)
                        lsSamplesBank_Pan[i].SelectedIndex = (int)val;

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

                        uint strToNum;

                        // range
                        strToNum = Convert.ToUInt32(lsSamplesBank_Start[i].Text);
                        srmFile[st1] = Convert.ToByte(strToNum >> 16);
                        srmFile[st2] = Convert.ToByte((strToNum >> 8) & 0xFF);
                        srmFile[st3] = Convert.ToByte(strToNum & 0xFF);


                        strToNum = Convert.ToUInt32(lsSamplesBank_End[i].Text);
                        srmFile[ed1] = Convert.ToByte(strToNum >> 16);
                        srmFile[ed2] = Convert.ToByte((strToNum >> 8) & 0xFF);
                        srmFile[ed3] = Convert.ToByte(strToNum & 0xFF);

                        // pan
                        srmFile[pan] = (byte)lsSamplesBank_Pan[i].SelectedIndex;

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
                Filter = "S-int 8 bit raw (*.raw)|*.raw|Any File (*.*)|*.*",
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
                        Name = ofd.SafeFileNames[i] + "_id_" + id.ToString(),
                        Text = id.ToString(),
                        Width = 40,
                        Location = new Point(5, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleCenter
                    };
                    samplesPool.Controls.Add(lbID);

                    Label lbFileName = new Label
                    {
                        Name = ofd.SafeFileNames[i] + "_name_" + id.ToString(),
                        Text = ofd.SafeFileNames[i],
                        Width = 300,
                        Location = new Point(50, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleLeft
                    };
                    samplesPool.Controls.Add(lbFileName);

                    FileInfo fi = new FileInfo(ofd.FileNames[i]);

                    sampleStart = sampleEnd + 1;
                    sampleEnd += fi.Length;

                    lsSamplesPool_Start.Add(sampleStart);
                    lsSamplesPool_End.Add(sampleEnd);

                    Label lbSampleStart = new Label
                    {
                        Name = ofd.SafeFileNames[i] + "_start_" + id.ToString(),
                        Text = sampleStart.ToString(),
                        Width = 80,
                        Location = new Point(360, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleRight
                    };
                    samplesPool.Controls.Add(lbSampleStart);

                    Label lbSampleEnd = new Label
                    {
                        Name = ofd.SafeFileNames[i] + "_end_" + id.ToString(),
                        Text = sampleEnd.ToString(),
                        Width = 80,
                        Location = new Point(450, y),
                        BorderStyle = BorderStyle.FixedSingle,
                        TextAlign = ContentAlignment.MiddleRight
                    };
                    samplesPool.Controls.Add(lbSampleEnd);

                    FileStream fs = new FileStream(ofd.FileNames[i], FileMode.Open, FileAccess.Read);
                    BinaryReader br = new BinaryReader(fs);

                    byte[] file = new byte[fs.Length];
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
