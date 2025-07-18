namespace KTANE.Backend.Serial;

public class OfflineSerialInterface : ISerialInterface
{
    public bool IsConnected() => true;

    public void Open(string portName, Action onDataReceived)
    {
    }

    public void Close() { }

    public byte ReadByte() => 0x66;
    public string ReadLine() => "NOTSUPPORTED";
    public void Write(byte data) { }
    public void Write(byte[] data)  { }
    public void DiscardBuffer()  { }
}
