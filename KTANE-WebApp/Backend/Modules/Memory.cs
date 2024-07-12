namespace KTANE_WebApp.Backend.Module;

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

        MemoryDigit display = displaySequence[currentStage];
        switch (currentStage)
        {
            case 0: // Stage 1
                switch (display)
                {
                    case MemoryDigit.ONE or MemoryDigit.TWO when index == 1:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.THREE when index == 2:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.FOUR when index == 3:
                        GoToNextStage(bomb);
                        return;
                }

                break;
            case 1: // Stage 2
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == MemoryDigit.FOUR:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.TWO or MemoryDigit.FOUR when index == buttonIndicesPressed[0]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.THREE when index == 0:
                        GoToNextStage(bomb);
                        return;
                }

                break;
            case 2: // Stage 3
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == buttonLabelsPressed[1]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.TWO when currentButtons[index] == buttonLabelsPressed[0]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.THREE when index == 2:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.FOUR when currentButtons[index] == MemoryDigit.FOUR:
                        GoToNextStage(bomb);
                        return;
                }

                break;
            case 3: // Stage 4
                switch (display)
                {
                    case MemoryDigit.ONE when index == buttonIndicesPressed[0]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.TWO when index == 0:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.THREE or MemoryDigit.FOUR when index == buttonIndicesPressed[1]:
                        GoToNextStage(bomb);
                        return;
                }

                break;
            case 4: // Stage 5
                switch (display)
                {
                    case MemoryDigit.ONE when currentButtons[index] == buttonLabelsPressed[0]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.TWO when currentButtons[index] == buttonLabelsPressed[1]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.THREE when currentButtons[index] == buttonLabelsPressed[3]:
                        GoToNextStage(bomb);
                        return;
                    case MemoryDigit.FOUR when currentButtons[index] == buttonLabelsPressed[2]:
                        GoToNextStage(bomb);
                        return;
                }

                break;
            default:
                throw new ArgumentOutOfRangeException();
        }

        bomb.IncrementTries();
        StartRandomState();
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

    public void Reset()
    {
        IsSolved = false;
        currentStage = 0;
        buttonIndicesPressed.Clear();
        buttonLabelsPressed.Clear();
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, CurrentStage: {currentStage}, CurrentBigNumber {displaySequence[currentStage]}, CurrentButtons ({string.Join(",", currentButtons)})";
    }

    public enum MemoryDigit : byte
    {
        ONE = 0,
        TWO = 1,
        THREE = 2,
        FOUR = 3
    }
}
