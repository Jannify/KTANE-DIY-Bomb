namespace KTANE_WebApp.Backend.Module;

public class Wires : IModule
{
    public bool IsSolved { get; private set; }

    public readonly WireColor[] wires = new WireColor[6];
    private readonly WireColor[] desiredState = new WireColor[6];

    public void Generate(Frame frame, Random random)
    {
        int wireCount = random.Next(3, 7);
        int wiresToDelete = 7 - wireCount;

        for (int i = 0; i < 6; i++)
        {
            wires[i] = desiredState[i] = (WireColor)random.Next(1, 6);
        }

        for (int n = 0; n < wiresToDelete; n++)
        {
            int i = random.Next(6);
            wires[i] = desiredState[i] = WireColor.NONE;
        }


        Func<WireColor, bool> hasWire = w => w != WireColor.NONE;
        Func<WireColor, bool> isBlack = w => w == WireColor.BLACK;
        Func<WireColor, bool> isBlue = w => w == WireColor.BLUE;
        Func<WireColor, bool> isRed = w => w == WireColor.RED;
        Func<WireColor, bool> isWhite = w => w == WireColor.WHITE;
        Func<WireColor, bool> isYellow = w => w == WireColor.YELLOW;

        switch (wireCount)
        {
            case 3:
                if (!wires.Contains(WireColor.RED))
                    desiredState.SetAt(hasWire, 2, WireColor.CUT);
                else if (wires.Last(hasWire) == WireColor.WHITE)
                    desiredState.SetAtLast(hasWire, WireColor.CUT);
                else if (wires.Count(isBlue) > 1)
                    desiredState.SetAtLast(isBlue, WireColor.CUT);
                else
                    desiredState.SetAtLast(hasWire, WireColor.CUT);
                return;
            case 4:
                if (wires.Count(isRed) > 1 && frame.SerialNumberLastDigit() % 2 == 1)
                    desiredState.SetAtLast(isRed, WireColor.CUT);
                else if (wires.Last(hasWire) == WireColor.YELLOW && !wires.Contains(WireColor.RED))
                    desiredState.SetAtFirst(hasWire, WireColor.CUT);
                else if (wires.Count(isBlue) == 1)
                    desiredState.SetAtFirst(hasWire, WireColor.CUT);
                else if (wires.Count(isYellow) > 1)
                    desiredState.SetAtLast(hasWire, WireColor.CUT);
                else
                    desiredState.SetAt(hasWire, 2, WireColor.CUT);
                return;
            case 5:
                if (wires.Last(hasWire) == WireColor.BLACK && frame.SerialNumberLastDigit() % 2 == 1)
                    desiredState.SetAt(hasWire, 4, WireColor.CUT);
                else if (wires.Count(isRed) == 1 && wires.Count(isYellow) > 1)
                    desiredState.SetAtFirst(hasWire, WireColor.CUT);
                else if (!wires.Any(isBlack))
                    desiredState.SetAt(hasWire, 2, WireColor.CUT);
                else
                    desiredState.SetAtFirst(hasWire, WireColor.CUT);
                return;
            case 6:
                if (!wires.Any(isYellow) && frame.SerialNumberLastDigit() % 2 == 1)
                    desiredState.SetAt(hasWire, 3, WireColor.CUT);
                else if (wires.Count(isYellow) == 1 && wires.Count(isWhite) > 1)
                    desiredState.SetAt(hasWire, 4, WireColor.CUT);
                else if (!wires.Any(isRed))
                    desiredState.SetAtLast(hasWire, WireColor.CUT);
                else
                    desiredState.SetAt(hasWire, 4, WireColor.CUT);
                return;
        }
    }

    public void HandleWireCut(Bomb bomb, int wireBitfield)
    {
        if(IsSolved)
            return;

        int wireCutIndex = -1;
        for (int i = 0; i < 5; i++)
        {
            if ((wireBitfield & 1) > 0)
            {
                wireCutIndex = i;
                break;
            }

            wireBitfield >>= 1;
        }

        wires[wireCutIndex] = WireColor.CUT;

        if (desiredState[wireCutIndex] != WireColor.CUT || desiredState[wireCutIndex] != WireColor.NONE)
        {
            bomb.IncrementTries();
            return;
        }

        if (!wires.Where((w, i) => w != desiredState[i]).Any())
        {
            IsSolved = true;
            bomb.UpdateSolvedModules();
        }
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, CurrentState ({string.Join(" | ", wires)}), CorrectState: ({string.Join(" | ", desiredState)})";
    }

    public enum WireColor
    {
        NONE,
        BLACK,
        BLUE,
        RED,
        WHITE,
        YELLOW,
        CUT
    }
}
