namespace KTANE_WebApp.Backend.Module;

public class Frame
{
    private readonly string[] indicatorTexts =
        ["SND", "CLR", "CAR", "IND", "FRQ", "SIG", "NSA", "MSA", "TRN", "BOB", "FRK"];

    private const string LETTERS = "ABCDEFGHIJKLMNEPQRSTUVWXZ";
    private const string VALID_DIGITS = "0123456789";
    private const string VOWELS = "AEIOU";

    public int NumberOfBatteries { get; private set; }
    public bool IndicatorLight { get; private set; }
    public string IndicatorText { get; private set; } = string.Empty;
    public string SerialNumber { get; private set; } = string.Empty;

    public void Generate(Random random)
    {
        NumberOfBatteries = random.Next(3);
        IndicatorLight = random.Next(2) > 0;
        IndicatorText = indicatorTexts[random.Next(indicatorTexts.Length)];

        // Copied from KTANE
        char[] possibleCharArray = (LETTERS + VALID_DIGITS).ToCharArray();
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
        return VOWELS.Any(vowel => SerialNumber.Contains(vowel));
    }

    public int SerialNumberLastDigit()
    {
        return int.Parse(SerialNumber.Last(c => VALID_DIGITS.Contains(c)).ToString());
    }

    public override string ToString()
    {
        return $"Batteries: {NumberOfBatteries}, IndicatorLight {IndicatorLight}, IndicatorText {IndicatorText}, SN: {SerialNumber}";
    }
}
