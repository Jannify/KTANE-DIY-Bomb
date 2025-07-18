using KTANE.Backend.Logger;

namespace KTANE.Backend.Modules;

public class BigButton : IModule
{
    public bool IsActive { get; set; }
    public bool IsSolved { get; private set; }

    public ColorOption ButtonColor;
    public ColorOption StripColor;
    public TextOption Text;

    public void Generate(Random random)
    {
        ButtonColor = (ColorOption)random.Next(4);
        StripColor = (ColorOption)random.Next(4);
        Text = (TextOption)random.Next(4);
    }

    public void HandleButtonPress(Bomb bomb, bool pressedLong, int minutesLeft, int secondsLeft)
    {
        if(!IsActive || IsSolved)
            return;

        Log.Debug($"[HandleBigButtonPress] Long:{pressedLong} SecondsLeft:{secondsLeft}");

        if (Text == TextOption.ABRECHEN && ButtonColor == ColorOption.BLUE)
            HandleShouldHold(bomb, minutesLeft, secondsLeft);
        else if (Text == TextOption.SPRENGEN && bomb.Frame.NumberOfBatteries > 1)
            HandleShouldPress(bomb, pressedLong);
        else if (ButtonColor == ColorOption.WHITE && bomb.Frame.IndicatorText == "CAR")
            HandleShouldHold(bomb, minutesLeft, secondsLeft);
        else if (bomb.Frame is { NumberOfBatteries: > 2, IndicatorLight: true, IndicatorText: "FRK" })
            HandleShouldPress(bomb, pressedLong);
        else if (ButtonColor == ColorOption.YELLOW)
            HandleShouldHold(bomb, minutesLeft, secondsLeft);
        else if (Text == TextOption.HALTEN && ButtonColor == ColorOption.RED)
            HandleShouldPress(bomb, pressedLong);
        else
            HandleShouldHold(bomb, minutesLeft, secondsLeft);
    }

    private void HandleShouldPress(Bomb bomb, bool pressedLong)
    {
        if (pressedLong)
        {
            SetSolved(bomb);
        }
        else
        {
            bomb.IncrementTries();
        }
    }

    private void HandleShouldHold(Bomb bomb, int minutesLeft, int secondsLeft)
    {
        string clockText = minutesLeft.ToString() + secondsLeft;

        switch (StripColor)
        {
            case ColorOption.BLUE when clockText.Contains('4'):
                SetSolved(bomb);
                return;
            case ColorOption.YELLOW when clockText.Contains('5'):
                SetSolved(bomb);
                return;
        }

        if (clockText.Contains('1'))
        {
            SetSolved(bomb);
            return;
        }

        bomb.IncrementTries();
    }

    private void SetSolved(Bomb bomb)
    {
        IsSolved = true;
        bomb.UpdateSolvedModules();
    }

    public void Reset()
    {
        IsSolved = false;
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, ButtonColor {ButtonColor}, StripColor {StripColor}, Text: {Text}";
    }

    public enum ColorOption
    {
        RED = 0,
        BLUE = 1,
        YELLOW = 2,
        WHITE = 3
    }

    public enum TextOption
    {
        ABRECHEN = 0,
        HALTEN = 1,
        SPRENGEN = 2,
        DRÜCKEN = 3
    }
}
