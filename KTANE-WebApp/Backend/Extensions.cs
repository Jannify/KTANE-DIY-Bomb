using System.Text;

namespace KTANE_WebApp.Backend;

public static class Extensions
{
    public static void Shuffle<T>(this IList<T> list, Random random)
    {
        int i = list.Count;
        while (i > 1)
        {
            i--;
            int num = random.Next(i + 1);
            (list[num], list[i]) = (list[i], list[num]);
        }
    }

    public static void SetAtFirst<T>(this IList<T> list, Func<T, bool> predicate, T value)
    {
        for (int i = 0; i < list.Count; i++)
        {
            if (predicate(list[i]))
            {
                list[i] = value;
                return;
            }
        }
    }

    public static void SetAtLast<T>(this IList<T> list, Func<T, bool> predicate, T value)
    {
        for (int i = list.Count - 1; i >= 0; i--)
        {
            if (predicate(list[i]))
            {
                list[i] = value;
                return;
            }
        }
    }

    public static void SetAt<T>(this IList<T> list, Func<T, bool> predicate, int occurence, T value)
    {
        int occurenceHappened = 0;
        for (int i = 0; i < list.Count; i++)
        {
            if (predicate(list[i]))
            {
                occurenceHappened++;
                if (occurenceHappened >= occurence)
                {
                    list[i] = value;
                    return;
                }
            }
        }
    }

    public static string BytesToHexString(this byte[] data)
    {
        StringBuilder sb = new();
        foreach (byte b in data)
        {
            sb.Append($"0x{b:X2} ");
        }

        return sb.ToString().TrimEnd();
    }
}
