using System.IO.Ports;
using KTANE_WebApp.Backend.Module;

namespace KTANE_WebApp.Backend;

public static class Arduino
{
    private static readonly SerialPort port = new();

    public static bool IsConnected => port.IsOpen;

    public static void Open(string portName)
    {
        port.PortName = portName;
        port.BaudRate = 9600;
        port.Open();
    }

    public static void Close() => port.Close();


    private static void Write(byte[] data)
    {
        port.Write(data, 0, data.Length);
    }

    /// <summary>
    /// Resets all configurations, powers every component down
    /// </summary>
    public static void ResetState()
    {
        Write([0x0]);
    }

    /// <summary>
    /// Init bomb with parameters
    /// </summary>
    public static void Init(string serialNumber, bool indicatorLight, string indicatorText, int morseIndex,
        byte bigButtonColorIndex, byte bigButtonTextIndex)
    {
        byte[] data = new byte[14];
        data[0] = 0x1;
        data[1] = (byte)serialNumber[0];
        data[2] = (byte)serialNumber[1];
        data[3] = (byte)serialNumber[2];
        data[4] = (byte)serialNumber[3];
        data[5] = (byte)serialNumber[4];
        data[6] = (byte)serialNumber[5];

        data[7] = (byte)(indicatorLight ? 1 : 0);
        data[8] = (byte)indicatorText[0];
        data[9] = (byte)indicatorText[1];
        data[10] = (byte)indicatorText[2];

        data[11] = (byte)morseIndex;

        data[12] = bigButtonColorIndex;
        data[13] = bigButtonTextIndex;
    }

    /// <summary>
    /// Starts the bomb
    /// </summary>
    public static void Start(ushort timeSeconds)
    {
        byte[] data = [0x2, (byte)(timeSeconds >> 8), (byte)timeSeconds];
        Write(data);
    }

    /// <summary>
    /// Updates the tries (X's on the bomb)
    /// </summary>
    public static void SetTries(byte newTriesCount)
    {
        byte[] data = [0x3, newTriesCount];
        Write(data);
    }

    /// <summary>
    /// Updates the solved components (just the green led)
    /// </summary>
    public static void SetSolved(bool[] modules)
    {
        byte[] data = new byte[3];
        data[0] = 0x4;
        data[1] = (byte)(modules[10] ? 0b00000100 : 0x00);
        data[1] &= (byte)(modules[9] ? 0b00000010 : 0x00);
        data[1] &= (byte)(modules[8] ? 0b00000001 : 0x00);

        for (int i = 7; i >= 0; i--)
        {
            data[2] &= (byte)(modules[i] ? 1 << i : 0x00);
        }

        Write(data);
    }

    /// <summary>
    /// Sets the color of BigButton Strip
    /// </summary>
    public static void SetBigButtonStrip(byte colorIndex)
    {
        byte[] data = [0x5, colorIndex];
        Write(data);
    }

    /// <summary>
    /// Sets Text of password module (5 letters)
    /// </summary>
    public static void SetPasswordText(char[] text)
    {
        byte[] data = [0x6, (byte)text[0], (byte)text[1], (byte)text[2], (byte)text[3], (byte)text[4]];
        Write(data);
    }

    /// <summary>
    /// Sets Text of password module (5 letters)
    /// </summary>
    public static void StartSimonSays(SimonSays.SimonSaysEntry[] sequence)
    {
        byte[] data = new byte[3];
        data[0] = 0x7;
        data[1] = (byte)((sequence.Length - 3) << 4);
        data[1] &= (byte)((byte)sequence[0] << 2);
        data[1] &= (byte)sequence[1];

        int offset = 6;
        for (int i = 2; i < sequence.Length; i++)
        {
            data[2] &= (byte)((byte)sequence[i] << offset);
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
        data[0] = 0x8;
        data[1] = (byte)((int)bigNumber << 6);
        data[1] &= (byte)((int)smallNumbers[0] << 4);
        data[1] &= (byte)((int)smallNumbers[1] << 2);
        data[1] &= (byte)((int)smallNumbers[2]);
        data[2] = (byte)((int)smallNumbers[3] << 6);
        data[2] &= (byte)(level << 3);

        Write(data);
    }
}
