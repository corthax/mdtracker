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

namespace mdteditor
{
    public partial class mdtEditor : Form
    {
        private static int ROM_SAMPLE_BANK = 0x0002F700;
        private static int ROM_SAMPLE_BANK_SIZE = 3399000;

        private static int SRM_FILE_SIZE = 524288;
        private static uint SRM_SAMPLE_DATA = 0x0726B;
        private static int SRM_SAMPLE_PAN = 0x6B0AB;
        private static int SRM_SAMPLE_RATE = 0x6F230;

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

        private byte[] srmFile;
        private List<Button> lsButtonsBank1;
        private List<TextBox> lsSampleStartBank1, lsSampleEndBank1, lsSampleIDBank1;
        private Dictionary<string, int> dicBtnID = new Dictionary<string, int>();
        private List<long> lsSampleStarts, lsSampleEnds;
        private Dictionary<int, byte[]> dicSampleFile = new Dictionary<int, byte[]>();
        private List<int> lsSampleSizes;
        private int samplesCount = 0;
        long sampleStart = -1, sampleEnd = 0;

        // GUI
        private static List<string> lNoteNames = new List<string> { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-" };
        private static int sampleSettingsHeight = 50;

        struct SampleBankPageElement
        {
            public Button btnSampleSync;
            public TextBox txtSampleStart;
            public TextBox txtSampleEnd;
            public ListBox txtSamplePan;
            public CheckBox txtSampleLoop;
            public ListBox txtSampleRate;
            public TextBox txtSampleID;
        }

        public mdtEditor()
        {
            InitializeComponent();
            srmFile = new byte[SRM_FILE_SIZE];
            lsButtonsBank1 = new List<Button>();
            lsSampleStartBank1 = new List<TextBox>();
            lsSampleEndBank1 = new List<TextBox>();
            lsSampleIDBank1 = new List<TextBox>();
            lsSampleStarts = new List<long>();
            lsSampleEnds = new List<long>();
            lsSampleSizes = new List<int>();
        }

        private void ButtonClicked(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            int id = dicBtnID[btn.Name];
            int syncID = -1;
            if (lsSampleIDBank1[id].Text != null)
            {
                int.TryParse(lsSampleIDBank1[id].Text, out syncID);

                if (syncID >= 0 && syncID < lsSampleStarts.Count)
                {
                    lsSampleStartBank1[id].Text = lsSampleStarts[syncID].ToString();
                    lsSampleEndBank1[id].Text = lsSampleEnds[syncID].ToString();
                }
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            for (int octave = 0; octave < 8; octave++)
            {
                for (int note = 0; note < 12; note++)
                {
                    int y;
                    y = 5 + (note * sampleSettingsHeight) + (12 * sampleSettingsHeight * octave);

                    string name;
                    name = "_Bank" + "0" + "_Octave" + octave.ToString() + "_Note" + note.ToString();

                    SampleBankPageElement pageElement = new SampleBankPageElement();

                    pageElement.btnSampleSync = new Button
                    {
                        Location = new Point(5, y),
                        Name = "btnSync" + name,
                        Width = 50,
                        Text = lNoteNames[note] + octave.ToString()
                    };
                    lsButtonsBank1.Add(pageElement.btnSampleSync);
                    tabBank1.Controls.Add(pageElement.btnSampleSync);

                    pageElement.txtSampleStart = new TextBox
                    {
                        Location = new Point(60, y),
                        Name = "txtStart" + name,
                        Width = 120,
                        Text = "",
                        TextAlign = HorizontalAlignment.Right
                    };
                    lsSampleStartBank1.Add(pageElement.txtSampleStart);
                    tabBank1.Controls.Add(pageElement.txtSampleStart);

                    pageElement.txtSampleEnd = new TextBox
                    {
                        Location = new Point(190, y),
                        Name = "txtEnd" + name,
                        Width = 120,
                        Text = "",
                        TextAlign = HorizontalAlignment.Right
                     };
                    lsSampleEndBank1.Add(pageElement.txtSampleEnd);
                    tabBank1.Controls.Add(pageElement.txtSampleEnd);

                    pageElement.txtSampleID = new TextBox
                    {
                        Location = new Point(320, y),
                        Name = "txtID" + name,
                        Width = 50,
                        Text = "0",
                        TextAlign = HorizontalAlignment.Right
                    };
                    lsSampleIDBank1.Add(pageElement.txtSampleID);
                    tabBank1.Controls.Add(pageElement.txtSampleID);
                }
            }

            for (int i = 0; i < 96; i++)
            {
                dicBtnID.Add(lsButtonsBank1[i].Name, i);
                lsButtonsBank1[i].Click += new EventHandler(ButtonClicked);
            }
        }

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

        // mdt sram. byteswapped
        private uint SRAMW_readByte(uint offset)
        {
            //if ((offset % 2) == 0) offset -= 2;
            if ((offset % 2) == 0) offset++; else offset--;
            return offset;
        }

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

                // load sample ranges
                for (int i = 0; i < lsSampleStartBank1.Count; i++)
                {
                    uint addr = SRM_SAMPLE_DATA + ((uint)i * 7); // 0x0726B

                    uint st1 = SRAMW_readByte(addr + 0);
                    uint st2 = SRAMW_readByte(addr + 1);
                    uint st3 = SRAMW_readByte(addr + 2);
                    uint ed1 = SRAMW_readByte(addr + 3);
                    uint ed2 = SRAMW_readByte(addr + 4);
                    uint ed3 = SRAMW_readByte(addr + 5);
                    uint lp  = SRAMW_readByte(addr + 6);

                    uint val;

                    val = ((uint)srmFile[st1] << 16) | (uint)(srmFile[st2] << 8) | (srmFile[st3]);
                    lsSampleStartBank1[i].Text = val.ToString();

                    val = ((uint)srmFile[ed1] << 16) | ((uint)srmFile[ed2] << 8) | (srmFile[ed3]);
                    lsSampleEndBank1[i].Text = val.ToString();

                    //int.TryParse(number, out int loop);
                    //lsSampleLoopBank1[i].Text = loop.ToString();
                }

                br.Close(); fs.Close();
            }
        }

        private void btnSaveRom_Click(object sender, EventArgs e)
        {
            // save sample bank data
            byte[] sampleData = new byte[ROM_SAMPLE_BANK_SIZE];
            int index = 0;
            for (int id = 0; id < lsSampleStarts.Count; id++)
            {
                dicSampleFile.TryGetValue(id, out byte[] sample);
                sample.CopyTo(sampleData, index);
                index += sample.Length;
            }

            if (txtRomName.Text!= "")
            {
                FileInfo fin = new FileInfo(txtRomName.Text);
                if (fin.Exists)
                {
                    FileStream fs = new FileStream(txtRomName.Text, FileMode.Open, FileAccess.Write);
                    BinaryWriter bw = new BinaryWriter(fs);

                    bw.Seek(ROM_SAMPLE_BANK, SeekOrigin.Begin);
                    bw.Write(sampleData, 0, ROM_SAMPLE_BANK_SIZE);

                    bw.Flush(); bw.Close(); fs.Close();
                }
            }

            // save presets

        }

        private void btnSaveSrm_Click(object sender, EventArgs e)
        {
            if (txtSrmName.Text != "")
            {
                FileInfo fin = new FileInfo(txtSrmName.Text);
                if (fin.Exists)
                {
                    FileStream fs = new FileStream(txtRomName.Text, FileMode.Open, FileAccess.Write);
                    BinaryWriter bw = new BinaryWriter(fs);

                    // save sample ranges and settings (pan, loop, rate)

                    //bw.Seek(ROM_SAMPLES_START, SeekOrigin.Begin);
                    //bw.Write(srmData, 0, ROM_SAMPLE_BANK_SIZE);

                    bw.Flush(); bw.Close(); fs.Close();
                }
            }
        }

        private void btnAddSamples_Click(object sender, EventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog
            {
                Filter = "U-int 8 bit raw (*.raw)|*.raw|Any File (*.*)|*.*",
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

                    lsSampleStarts.Add(sampleStart);
                    lsSampleEnds.Add(sampleEnd);

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

                    dicSampleFile.Add(id, file);
                    lsSampleSizes.Add(file.Length);

                    br.Close(); fs.Close();
                }
                txtBytesUsed.Text = sampleEnd.ToString();
                samplesCount += ofd.SafeFileNames.Length;
            }
        }
    }
}
