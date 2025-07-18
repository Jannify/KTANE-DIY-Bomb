namespace KTANE.Backend.Serial;

public interface ISerialInterface
{
    public bool IsConnected();

    public void Open(string portName, Action onDataReceived);

    public void Close();

    public byte ReadByte();
    public string ReadLine();
    public void Write(byte data);
    public void Write(byte[] data);
    public void DiscardBuffer();
}
