using KTANE.Backend.Logger;

namespace KTANE.Backend.Modules;

public class Memory : IModule
{
    public bool IsActive { get; set; }
    public bool IsSolved { get; private set; }

    private Random inStageRandom;
    private int currentStage;
    private readonly List<int> buttonIndicesPressed = [];
    private readonly List<MemoryDigit> buttonLabelsPressed = [];
    private readonly MemoryDigit[] displaySequence = new MemoryDigit[5];
    private readonly MemoryDigit[] currentButtons = new MemoryDigit[4];

    public void Generate(Random random)
    {
        inStageRandom = new Random(random.Next());
    }

    public void HandleButtonPressed(Bomb bomb, int index)
    {
        if(!IsActive || IsSolved)
            return;

        buttonIndicesPressed.Add(index);
        buttonLabelsPressed.Add(currentButtons[index]);

        if (IsCorrectButton(index))
        {
            GoToNextStage(bomb);
        }
        else
        {
            bomb.IncrementTries();
            StartRandomState();
        }

        Log.Debug($"[{nameof(Memory),9}] {ToString()}");
    }

    private bool IsCorrectButton(int index)
    {
        MemoryDigit display = displaySequence[currentStage];
        switch (currentStage)
        {
            case 0: // Stage 1
                switch (display)
                {
                    case MemoryDigit.ONE or MemoryDigit.TWO when index == 1:
                    case MemoryDigit.THREE when index == 2:
                    case MemoryDigit.FOUR when index == 3:
                        return true;
                }

                break;
            case 1: // Stage 2
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == MemoryDigit.FOUR:
                    case MemoryDigit.TWO or MemoryDigit.FOUR when index == buttonIndicesPressed[0]:
                    case MemoryDigit.THREE when index == 0:
                        return true;
                }

                break;
            case 2: // Stage 3
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == buttonLabelsPressed[1]:
                    case MemoryDigit.TWO when currentButtons[index] == buttonLabelsPressed[0]:
                    case MemoryDigit.THREE when index == 2:
                    case MemoryDigit.FOUR when currentButtons[index] == MemoryDigit.FOUR:
                        return true;
                }

                break;
            case 3: // Stage 4
                switch (display)
                {
                    case MemoryDigit.ONE when index == buttonIndicesPressed[0]:
                    case MemoryDigit.TWO when index == 0:
                    case MemoryDigit.THREE or MemoryDigit.FOUR when index == buttonIndicesPressed[1]:
                        return true;
                }

                break;
            case 4: // Stage 5
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == buttonLabelsPressed[0]:
                    case MemoryDigit.TWO when currentButtons[index] == buttonLabelsPressed[1]:
                    case MemoryDigit.THREE when currentButtons[index] == buttonLabelsPressed[3]:
                    case MemoryDigit.FOUR when currentButtons[index] == buttonLabelsPressed[2]:
                        return true;
                }

                break;
            default:
                throw new ArgumentOutOfRangeException();
        }

        return false;
    }

    public void StartRandomState()
    {
        currentStage = 0;
        buttonIndicesPressed.Clear();
        buttonLabelsPressed.Clear();

        for (int i = 0; i < displaySequence.Length; i++)
        {
            displaySequence[i] = (MemoryDigit)inStageRandom.Next(1, 4);
        }

        ShuffleButtonLabels(inStageRandom);
    }

    private void GoToNextStage(Bomb bomb)
    {
        currentStage++;
        if (currentStage >= 5)
        {
            IsSolved = true;
            bomb.UpdateSolvedModules();
            Arduino.SetMemory(MemoryDigit.ONE, currentButtons, currentStage);
            return;
        }

        ShuffleButtonLabels(inStageRandom);
    }

    private void ShuffleButtonLabels(Random random)
    {
        List<MemoryDigit> list = [MemoryDigit.ONE, MemoryDigit.TWO, MemoryDigit.THREE, MemoryDigit.FOUR];
        list.Shuffle(random);
        for (int i = 0; i < currentButtons.Length; i++)
        {
            currentButtons[i] = list[i];
        }

        Arduino.SetMemory(displaySequence[currentStage], currentButtons, currentStage);
    }

    public MemoryDigit GetCorrectButton()
    {
        int correctButton = -1;
        for (int i = 0; i < 4; i++)
        {
            if (IsCorrectButton(i))
            {
                if(correctButton != -1)
                    Log.Error($"Multiple correct buttons found:\n" +
                              $"Button: {i},\n currentButtons: ({string.Join(",", currentButtons)}),\n" +
                              $"buttonIndicesPressed: ({string.Join(",", buttonIndicesPressed)}),\n buttonLabelsPressed: ({string.Join(",", buttonLabelsPressed)})");

                correctButton = i;
            }
        }

        return (MemoryDigit)correctButton;
    }

    public void Reset()
    {
        IsSolved = false;
        currentStage = 0;
        buttonIndicesPressed.Clear();
        buttonLabelsPressed.Clear();
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, CurrentStage: {currentStage}, BigNumber {displaySequence[currentStage]}, Buttons ({string.Join(",", currentButtons)}), CorrectButton: {GetCorrectButton()}";
    }

    public enum MemoryDigit : byte
    {
        ONE = 0,
        TWO = 1,
        THREE = 2,
        FOUR = 3
    }
}
