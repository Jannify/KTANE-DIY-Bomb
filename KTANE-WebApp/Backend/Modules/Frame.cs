namespace KTANE_WebApp.Backend.Module;

public class Frame : IModule
{
    private readonly string[] indicatorTexts =
        ["SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"];

    private const string letters = "ABCDEFGHIJKLMNEPQRSTUVWXZ";
    private const string validDigits = "0123456789";
    private const string vowels = "AEIOU";

    public bool IsSolved => true;
    public int NumberOfBatteries { get; private set; }
    public bool IndicatorLight { get; private set; }
    public string IndicatorText { get; private set; }
    public string SerialNumber { get; private set; }

    public void Generate(Random random)
    {
        NumberOfBatteries = random.Next(3);
        IndicatorLight = random.Next(2) > 0;
        IndicatorText = indicatorTexts[random.Next(indicatorTexts.Length)];

        // Copied from KTANE
        char[] possibleCharArray = (letters + validDigits).ToCharArray();
        string text = string.Empty;
        for (int i = 0; i < 2; i++)
        {
            text += possibleCharArray[random.Next(possibleCharArray.Length)];
        }

        text += random.Next(10);
        for (int j = 3; j < 6; j++)
        {
            text += possibleCharArray[random.Next(possibleCharArray.Length - 10)];
        }

        text += random.Next(10);
        SerialNumber = text;
    }

    public bool SerialNumberContainsVowel()
    {
        return vowels.Any(vowel => SerialNumber.Contains(vowel));
    }

    public int SerialNumberLastDigit()
    {
        return int.Parse(SerialNumber.Last(c => validDigits.Contains(c)).ToString());
    }

    public override string ToString()
    {
        return $"Batteries: {NumberOfBatteries}, IndicatorLight {IndicatorLight}, IndicatorText {IndicatorText}, SN: {SerialNumber}";
    }
}
