using System.Text;

namespace KTANE_WebApp;

public static class Utils
{
    public static string BytesToHexString(byte[] data)
    {
        StringBuilder sb = new();
        foreach (byte b in data)
        {
            sb.Append($"0x{b:X2} ");
        }

        return sb.ToString().TrimEnd();
    }
}
