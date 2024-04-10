using System.Timers;
using Timer = System.Timers.Timer;

namespace KTANE_WebApp.Backend.Module;

public class SimonSays : IModule
{
    public bool IsSolved { get; private set; }

    private Timer restartTimer;
    private SimonSaysEntry[] sequence;

    private bool hasVowel;
    private int currentSequenceLength;
    private int pressedButtonCount;


    public void Generate(Frame frame, Random random)
    {
        hasVowel = frame.SerialNumberContainsVowel();
        currentSequenceLength = 0;
        pressedButtonCount = 0;

        int sequenceLength = random.Next(3, 6);
        sequence = new SimonSaysEntry[sequenceLength];
        for (int i = 0; i < sequenceLength; i++)
        {
            sequence[i] = (SimonSaysEntry)random.Next(0, 4);
        }
    }

    public void InitSequence()
    {
        restartTimer = new Timer(7000);
        restartTimer.Elapsed += RestartTimerCallback;
        restartTimer.AutoReset = true;
        restartTimer.Enabled = true;
    }

    private void RestartTimerCallback(object? source, ElapsedEventArgs e)
    {
        if (currentSequenceLength == 0)
            RunSequence(0);
    }

    public void HandleButtonPressed(Bomb bomb, byte buttonIndex)
    {
        if (IsSolved)
            return;

        restartTimer.Stop();

        SimonSaysEntry sequenceEntry = sequence[pressedButtonCount];
        if (GetCorrectButtonPress(bomb.Mistakes, hasVowel, sequenceEntry) != (SimonSaysEntry)buttonIndex)
        {
            currentSequenceLength = 0;
            pressedButtonCount = 0;
            bomb.IncrementTries();
            restartTimer.Start();
            RunSequence(1500);
            return;
        }

        pressedButtonCount++;

        if (pressedButtonCount < currentSequenceLength)
            return;

        currentSequenceLength++;

        if (currentSequenceLength >= sequence.Length)
        {
            IsSolved = true;
            restartTimer.Dispose();
            bomb.UpdateSolvedModules();
            return;
        }

        pressedButtonCount = 0;
        RunSequence(1000);
    }

    private void RunSequence(int delay)
    {
        Task.Run(() =>
        {
            Thread.Sleep(delay);
            Arduino.StartSimonSays(sequence.Take(currentSequenceLength + 1).ToArray());
        });
    }


    private static SimonSaysEntry GetCorrectButtonPress(int mistakes, bool hasVowel, SimonSaysEntry sequenceEntry)
    {
        if (hasVowel)
        {
            switch (sequenceEntry)
            {
                case SimonSaysEntry.BLUE:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.RED;
                        case 1:
                            return SimonSaysEntry.GREEN;
                        case 2:
                            return SimonSaysEntry.RED;
                    }

                    break;
                case SimonSaysEntry.RED:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.BLUE;
                        case 1:
                            return SimonSaysEntry.YELLOW;
                        case 2:
                            return SimonSaysEntry.GREEN;
                    }

                    break;
                case SimonSaysEntry.YELLOW:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.GREEN;
                        case 1:
                            return SimonSaysEntry.RED;
                        case 2:
                            return SimonSaysEntry.BLUE;
                    }

                    break;
                case SimonSaysEntry.GREEN:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.YELLOW;
                        case 1:
                            return SimonSaysEntry.BLUE;
                        case 2:
                            return SimonSaysEntry.YELLOW;
                    }

                    break;
            }
        }
        else // No vowel
        {
            switch (sequenceEntry)
            {
                case SimonSaysEntry.BLUE:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.YELLOW;
                        case 1:
                            return SimonSaysEntry.BLUE;
                        case 2:
                            return SimonSaysEntry.GREEN;
                    }

                    break;
                case SimonSaysEntry.RED:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.BLUE;
                        case 1:
                            return SimonSaysEntry.RED;
                        case 2:
                            return SimonSaysEntry.YELLOW;
                    }

                    break;
                case SimonSaysEntry.YELLOW:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.RED;
                        case 1:
                            return SimonSaysEntry.GREEN;
                        case 2:
                            return SimonSaysEntry.RED;
                    }

                    break;
                case SimonSaysEntry.GREEN:
                    switch (mistakes)
                    {
                        case 0:
                            return SimonSaysEntry.GREEN;
                        case 1:
                            return SimonSaysEntry.YELLOW;
                        case 2:
                            return SimonSaysEntry.BLUE;
                    }

                    break;
            }
        }

        throw new Exception($"Reached end of {nameof(GetCorrectButtonPress)} without entering a switch statement");
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, CurrentSequence ({string.Join(" | ", sequence)}), AlreadyPressedButtonCount: {pressedButtonCount}";
    }

    public enum SimonSaysEntry
    {
        BLUE = 0,
        RED = 1,
        YELLOW = 2,
        GREEN = 3
    }
}
