using System.Text;
using KTANE_WebApp.Backend.Module;

namespace KTANE_WebApp.Backend;

public class Bomb
{
    public static readonly Bomb Instance = new();

    private ushort initialTime;
    public int MaxMistakes;
    public int Mistakes;

    private IModule[] modules = [];
    public readonly Frame Frame = new();
    private readonly BigButton bigButton = new();
    private readonly Wires wires = new();
    private readonly Password password = new();
    private readonly SimonSays simonSays = new();
    private readonly Memory memory = new();
    private readonly Morse morse = new();

    private Bomb() => Arduino.OnMessageReceived += HandleSerialInput;

    ~Bomb() => Arduino.OnMessageReceived -= HandleSerialInput;

    public void GenerateNewBomb(BombGenerationInputInfo info)
    {
        Random random = new(info.Seed);
        initialTime = info.InitialTime;
        MaxMistakes = info.MaxMistakes;
        Mistakes = Math.Abs(3 - MaxMistakes);

        modules = [bigButton, wires, morse, memory, simonSays, password];
        foreach (IModule module in modules)
        {
            module.Reset();
            module.IsActive = true;
        }

        Frame.Generate(random);
        bigButton.Generate(random);
        wires.Generate(Frame, random);
        password.Generate(random);
        simonSays.Generate(Frame, random);
        memory.Generate(random);
        morse.Generate(random);

        Arduino.ResetState();
        Thread.Sleep(10);

        Arduino.InitFrame(Frame.SerialNumber, Frame.IndicatorLight, Frame.IndicatorText);
        Thread.Sleep(10);

        bool[] activeModules = new bool[14];
        for (int i = 0; i < modules.Length; i++)
        {
            activeModules[i] = modules[i].IsActive;
        }
        Arduino.InitStaticModules(activeModules, morse.MorseIndex, (byte)bigButton.ButtonColor, (byte)bigButton.Text);
        Thread.Sleep(10);

        Arduino.SetTries(Mistakes);
        UpdateSolvedModules();
        Thread.Sleep(10);

        Arduino.SetBigButtonStrip((byte)bigButton.StripColor);
        Thread.Sleep(10);

        Arduino.SetPasswordText(password.GetCurrentWord());
        Thread.Sleep(10);

        memory.StartRandomState();
    }

    public void Start()
    {
        Arduino.Start(initialTime);
        simonSays.InitSequence();
    }


    private void HandleSerialInput(byte[] data)
    {
        switch (data[0])
        {
            case 0x1: // BigButton
                bool pressedLong = data[1] > 0;
                ushort secondsLeft = BitConverter.ToUInt16(data, 2);
                bigButton.HandleButtonPress(this, pressedLong, secondsLeft);
                break;
            case 0x2: //Wire
                wires.HandleWireCut(this, data[1]);
                break;
            case 0x3: // Password
                bool send = (data[1] & 0b10000000) > 0;
                bool isUp = (data[1] & 0b01000000) > 0;
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
                if (morse.IsSolved)
                    UpdateSolvedModules();
                else
                    IncrementTries();
                break;
            case 0xE: // Time Up
                Explode();
                break;
        }
    }

    public void IncrementTries()
    {
        Mistakes++;
        Arduino.SetTries(Mistakes);

        if (Mistakes >= MaxMistakes)
            Explode();
    }

    public void UpdateSolvedModules()
    {
        bool[] solved = new bool[14];
        for (int i = 0; i < modules.Length; i++)
        {
            solved[i] = modules[i].IsSolved;
        }

        Arduino.SetSolved(solved);
    }

    private void Explode()
    {
        Arduino.Explode();
        simonSays.Stop();
        Console.WriteLine("BOMB EXPLODE");
    }

    public Wires GetWiresModule() => wires;

    public void ResetState()
    {
        foreach (IModule module in modules)
        {
            module.Reset();
        }
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
