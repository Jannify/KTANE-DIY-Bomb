using System.IO.Ports;

namespace KTANE.Backend.Serial;

public class SerialInterface : ISerialInterface
{
    private static readonly SerialPort port = new();

    public bool IsConnected() => port.IsOpen;

    public void Open(string portName, Action onDataReceived)
    {
        port.PortName = portName;
        port.BaudRate = 9600;
        port.DataReceived += (_, _) => onDataReceived.Invoke();
        port.Open();
        port.DiscardOutBuffer();
        port.DiscardInBuffer();
    }

    public void Close() => port.Close();

    public byte ReadByte() => (byte)port.ReadByte();
    public string ReadLine() => port.ReadLine();
    public void Write(byte data) => port.Write([data], 0, 1);
    public void Write(byte[] data) => port.Write(data, 0, data.Length);
    public void DiscardBuffer() => port.DiscardInBuffer();
}
