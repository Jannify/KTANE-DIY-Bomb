using System.Text;
using KTANE_WebApp.Backend.Module;

namespace KTANE_WebApp.Backend;

public class Bomb
{
    public static Bomb Instance = new();

    private ushort initialTime;
    public int MaxMistakes;
    public int Mistakes;

    private IModule[] modules;
    public readonly Frame Frame = new();
    private readonly BigButton bigButton = new();
    public readonly Wires wires = new();
    private readonly Password password = new();
    private readonly SimonSays simonSays = new();
    private readonly Memory memory = new();
    private readonly Morse morse = new();


    public void GenerateNewBomb(BombGenerationInputInfo info)
    {
        Random random = new(info.Seed);
        initialTime = info.InitialTime;
        MaxMistakes = info.MaxMistakes;
        Mistakes = 0;

        modules = [bigButton, wires, password, simonSays, memory, morse];
        Frame.Generate(random);
        bigButton.Generate(random);
        wires.Generate(Frame, random);
        password.Generate(random);
        simonSays.Generate(Frame, random);
        memory.Generate(random);
        morse.Generate(random);

        Arduino.ResetState();
        Arduino.Init(Frame.SerialNumber, Frame.IndicatorLight, Frame.IndicatorText,
            morse.MorseIndex, (byte)bigButton.ButtonColor, (byte)bigButton.Text);

        Arduino.SetBigButtonStrip((byte)bigButton.StripColor);
        Arduino.SetPasswordText(password.GetCurrentWord());
        memory.StartRandomState();
    }

    public void Start()
    {
        Arduino.Start(initialTime);
        simonSays.InitSequence();
    }


    public void HandleSerialInput(byte[] data)
    {
        switch (data[0])
        {
            case 0x1: // BigButton
                bool timeDown = data[1] > 0;
                ushort secondsLeft = data[3];
                secondsLeft &= (ushort)(data[2] << 8);
                bigButton.HandleButtonPress(this, timeDown, secondsLeft);
                break;
            case 0x2: //Wire
                wires.HandleWireCut(this, data[1]);
                break;
            case 0x3: // Password
                bool send = (data[1] & 1) > 0;
                bool isUp = (data[1] & 2) > 0;
                int index = (data[1] & 0b00111000) >> 3;
                password.HandelButtonPress(this, send, isUp, index);
                break;
            case 0x4: // SimonSays
                simonSays.HandleButtonPressed(this, data[1]);
                break;
            case 0x5: // Memory
                memory.HandleButtonPressed(this, data[1]);
                break;
            case 0x6: // Morse
                morse.IsSolved = data[1] > 0;
                if (simonSays.IsSolved)
                    UpdateSolvedModules();
                else
                    IncrementTries();
                break;
        }
    }

    public void IncrementTries()
    {
        Mistakes++;

        if (Mistakes >= MaxMistakes)
        {
            Explode();
            Arduino.ResetState();
        }
    }

    public void UpdateSolvedModules()
    {
        bool[] solved = new bool[11];
        for (int i = 0; i < modules.Length; i++)
        {
            solved[i] = modules[i].IsSolved;
        }

        Arduino.SetSolved(solved);
    }

    private void Explode()
    {
        // Play explosion Sound
    }

    public override string ToString()
    {
        StringBuilder sb = new("KTANE Bomb:\n");
        sb.AppendLine($"   MaxMistakes: {MaxMistakes}, Mistakes: {Mistakes}, Time: {initialTime}");
        sb.AppendLine($"   [    Frame] {Frame}");

        foreach (IModule module in modules)
        {
            sb.AppendLine($"   [{module.GetType().Name,9}] {module}");
        }

        return sb.ToString();
    }
}
