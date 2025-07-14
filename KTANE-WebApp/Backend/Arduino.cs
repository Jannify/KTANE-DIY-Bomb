using System.IO.Ports;
using System.Runtime.InteropServices;
using KTANE.Backend.Logger;
using KTANE.Backend.Modules;

namespace KTANE.Backend;

public static class Arduino
{
    public delegate void OnMessageReceivedHandler(byte[] data);

    public static event OnMessageReceivedHandler? OnMessageReceived;


    private static readonly SerialPort port = new();

    public static bool IsConnected => port.IsOpen;

    public static void Open()
    {
        try
        {
            if (RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            {
                try
                {
                    Open("/dev/ttyACM0");
                }
                catch (Exception _)
                {
                    Open("/dev/ttyACM1");
                }
            }
            else
            {
                Open("COM3");
            }
        }
        catch (Exception ex)
        {
            Log.Error(ex);
        }
    }

    public static void Open(string portName)
    {
        if (IsConnected)
        {
            Log.Warn("[Arduino] Port already open");
            return;
        }

        port.PortName = portName;
        port.BaudRate = 9600;
        port.DataReceived += OnDataReceived;
        port.Open();
        port.DiscardOutBuffer();
        port.DiscardInBuffer();
        Log.Info("[Arduino] Port opened");
    }

    public static void Close()
    {
        port.Close();
        Log.Info("[Arduino] Port closed");
    }


    private static void Write(byte[] data)
    {
        Log.Debug($"Send: {data.BytesToHexString()}");
        port.Write(data, 0, data.Length);
    }

    private static void OnDataReceived(object sender, SerialDataReceivedEventArgs e)
    {
        try
        {
            int type = port.ReadByte();

            byte[] data;
            switch (type)
            {
                case 0x1: // BigButton Down
                    data = [(byte)type, 0, 0, 0];
                    data[1] = (byte)port.ReadByte();
                    data[2] = (byte)port.ReadByte();
                    data[3] = (byte)port.ReadByte();
                    break;
                case 0x2: // Wire Cut
                case 0x3: // Password Changes
                case 0x4: // SimonSays
                case 0x5: // Memory
                case 0x6: // Morse Code
                    data = [(byte)type, 0];
                    data[1] = (byte)port.ReadByte();
                    break;
                case 0xE: // Time Up
                    data = [(byte)type];
                    break;
                case 0xF: //Logging
                    //string msg = port.ReadTo("\0");
                    string msg = port.ReadLine();
                    Log.Error("[Arduino] " + msg);
                    return;
                default:
                    port.DiscardInBuffer();
                    Write([0xE]);
                    Log.Error($"Serial data type ({type}) was not in expected range (0x1-0x6 + 0xE + 0xF). Requesting resend");
                    return;
            }


            Log.Debug($"Received: {data.BytesToHexString()}");
            OnMessageReceived?.Invoke(data);
        }
        catch (Exception ex)
        {
            Log.Error(ex);
        }
    }

    /// <summary>
    /// Resets all configurations, powers every component down
    /// </summary>
    public static void ResetState()
    {
        Write([0x0]);
    }

    /// <summary>
    /// Init bomb frame with parameters
    /// </summary>
    public static void InitFrame(string serialNumber, bool indicatorLight, string indicatorText)
    {
        byte[] data =
        [
            0x1,
            (byte)serialNumber[0],
            (byte)serialNumber[1],
            (byte)serialNumber[2],
            (byte)serialNumber[3],
            (byte)serialNumber[4],
            (byte)serialNumber[5],

            (byte)(indicatorLight ? 1 : 0),
            (byte)indicatorText[0],
            (byte)indicatorText[1],
            (byte)indicatorText[2],
        ];
        Write(data);
    }

    /// <summary>
    /// Init bomb static modules with parameters
    /// </summary>
    public static void InitStaticModules(bool[] activeModules, int morseIndex, byte bigButtonColorIndex, byte bigButtonTextIndex)
    {
        byte[] data =
        [
            0x2,
            0,
            (byte)morseIndex,
            bigButtonColorIndex,
            bigButtonTextIndex
        ];

        for (int i = 0; i < 7; i++)
        {
            data[1] |= (byte)(activeModules[i] ? 0b000000001 << i : 0x00);
        }

        Write(data);
    }

    /// <summary>
    /// Starts the bomb
    /// </summary>
    public static void Start(ushort timeSeconds)
    {
        byte[] data = [0x3, (byte)(timeSeconds >> 8), (byte)timeSeconds];
        Write(data);
    }

    /// <summary>
    /// Updates the tries (X's on the bomb)
    /// </summary>
    public static void SetTries(int newTriesCount)
    {
        byte[] data = [0x4, (byte)newTriesCount];
        Write(data);
    }

    /// <summary>
    /// Updates the solved components (just the green led)
    /// </summary>
    public static void SetSolved(bool[] modules)
    {
        byte[] data = new byte[2];
        data[0] = 0x5;

        for (int i = 0; i < 7; i++)
        {
            data[1] |= (byte)(modules[i] ? 0b000000001 << i : 0x00);
        }

        Write(data);
    }

    /// <summary>
    /// Sets the color of BigButton Strip
    /// </summary>
    public static void SetBigButtonStrip(byte colorIndex)
    {
        byte[] data = [0x6, colorIndex];
        Write(data);
    }

    /// <summary>
    /// Sets Text of password module (5 letters)
    /// </summary>
    public static void SetPasswordText(char[] text)
    {
        byte[] data = [0x7, (byte)text[0], (byte)text[1], (byte)text[2], (byte)text[3], (byte)text[4]];
        Write(data);
    }

    /// <summary>
    /// Sets Text of password module (5 letters)
    /// </summary>
    public static void StartSimonSays(SimonSays.SimonSaysEntry[] sequence)
    {
        byte[] data = new byte[3];
        data[0] = 0x8;
        data[1] = (byte)(sequence.Length << 4);
        data[1] |= (byte)((byte)sequence[0] << 2);

        if (sequence.Length > 1)
        {
            data[1] |= (byte)sequence[1];
        }

        int offset = 6;
        for (int i = 2; i < sequence.Length && i < 6; i++)
        {
            data[2] |= (byte)((byte)sequence[i] << offset);
            offset -= 2;
        }

        Write(data);
    }

    /// <summary>
    /// Sets Memory 7Bit-segments
    /// </summary>
    /// <remarks><see cref="level"/> goes from 0 to 5</remarks>
    public static void SetMemory(Memory.MemoryDigit bigNumber, Memory.MemoryDigit[] smallNumbers, int level)
    {
        byte[] data = new byte[3];
        data[0] = 0x9;
        data[1] = (byte)((int)bigNumber << 6);
        data[1] |= (byte)((int)smallNumbers[0] << 4);
        data[1] |= (byte)((int)smallNumbers[1] << 2);
        data[1] |= (byte)((int)smallNumbers[2]);
        data[2] = (byte)((int)smallNumbers[3] << 6);

        switch (level)
        {
            case 0:
                break;
            case 1:
                data[2] |= 0b00000010;
                break;
            case 2:
                data[2] |= 0b00000110;
                break;
            case 3:
                data[2] |= 0b00001110;
                break;
            case 4:
                data[2] |= 0b00011110;
                break;
            case 5:
                data[2] |= 0b00111110;
                break;
            default:
                throw new ArgumentException("level was not in range of 0-5");
        }

        Write(data);
    }

    public static void Explode() => Write([0xF]);
}
