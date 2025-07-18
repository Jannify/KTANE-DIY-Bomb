using System.Text;
using KTANE.Backend.Logger;
using KTANE.Backend.Modules;
using NetCoreAudio;

namespace KTANE.Backend;

public class Bomb
{
    public static readonly Bomb Instance = new();

    private readonly Player soundplayer = new();
    private int soundVolume = 25;

    private ushort initialTime;
    public int MaxMistakes;
    public int Mistakes;
    public bool Exploded;
    public bool Solved;

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
        Exploded = false;

        modules = [bigButton, wires, morse, memory, simonSays, password];
        foreach (IModule module in modules)
        {
            module.Reset();
            if (info.ActiveModules[module.GetType()])
            {
                module.IsActive = true;
            }
        }

        Frame.Generate(random);
        bigButton.Generate(random);
        wires.Generate(Frame, random);
        password.Generate(random);
        simonSays.Generate(Frame, random);
        memory.Generate(random);
        morse.Generate(random);

        Arduino.ResetState();
        Thread.Sleep(100);

        Arduino.InitFrame(Frame.SerialNumber, Frame.IndicatorLight, Frame.IndicatorText);
        Thread.Sleep(100);

        bool[] activeModules = new bool[14];
        for (int i = 0; i < modules.Length; i++)
        {
            activeModules[i] = modules[i].IsActive;
        }

        Arduino.InitStaticModules(activeModules, morse.MorseIndex, (byte)bigButton.ButtonColor, (byte)bigButton.Text);
        Thread.Sleep(100);

        Arduino.SetTries(Mistakes);
        UpdateSolvedModules();
        Thread.Sleep(100);

        Arduino.SetBigButtonStrip((byte)bigButton.StripColor);
        Thread.Sleep(100);

        Arduino.SetPasswordText(password.GetCurrentWord());
        Thread.Sleep(100);

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
                ushort combinedSecondsLeft = BitConverter.ToUInt16(data, 2);
                int minutesLeft = combinedSecondsLeft / 60;
                int secondsLeft = combinedSecondsLeft - minutesLeft * 60;
                bigButton.HandleButtonPress(this, pressedLong, minutesLeft, secondsLeft);
                Log.Debug($"[BigButton] Pressed button {(pressedLong ? "long" : "short")} at {minutesLeft}:{secondsLeft}");
                break;
            case 0x2: //Wire
                wires.HandleWireCut(this, data[1]);
                Log.Debug($"[Wires] Cut wire at {data[1]}");
                break;
            case 0x3: // Password
                bool send = (data[1] & 0b10000000) > 0;
                bool isUp = (data[1] & 0b01000000) > 0;
                int index = (data[1] & 0b00111000) >> 3;
                password.HandelButtonPress(this, send, isUp, index);
                if (send)
                    Log.Debug("[Password] Send");
                else
                    Log.Debug($"[Password] Pressed {(isUp ? "Up" : "Down")} at {index}");
                break;
            case 0x4: // SimonSays
                simonSays.HandleButtonPressed(this, data[1]);
                Log.Debug($"[SimonSays] Pressed {(SimonSays.SimonSaysEntry)data[1]}");
                break;
            case 0x5: // Memory
                memory.HandleButtonPressed(this, data[1]);
                Log.Debug($"[Memory] Pressed button at index {data[1]}");
                break;
            case 0x6: // Morse
                morse.IsSolved = data[1] > 0;
                if (morse.IsSolved)
                    UpdateSolvedModules();
                else
                    IncrementTries();
                Log.Debug($"[Morse] Sent. Was solved: {morse.IsSolved}");
                break;
            case 0xD: // Time Up
                Explode();
                break;
        }
    }

    public void IncrementTries()
    {
        if (Mistakes + 1 >= MaxMistakes)
            Explode();
        else
        {
            Mistakes++;
            Arduino.SetTries(Mistakes);
            PlayMistakeSound();
        }
    }

    public void UpdateSolvedModules()
    {
        bool[] solved = new bool[14];
        for (int i = 0; i < modules.Length; i++)
        {
            solved[i] = modules[i].IsSolved;
        }

        Arduino.SetSolved(solved);

        foreach (IModule module in modules)
        {
            if (module is { IsActive: true, IsSolved: false })
                return;
        }

        BombSolved();
    }

    public int SoundVolume
    {
        get => soundVolume;
        set
        {
            soundVolume = value;
            soundplayer.SetVolume((byte)value);
        }
    }

    public void PlayMistakeSound()
    {
        soundplayer.Play(Path.Combine("Sounds", "strike.wav"));
    }

    public void Explode()
    {
        Exploded = true;
        Arduino.Explode();
        soundplayer.Play(Path.Combine("Sounds", "explosion stones.wav"));
        simonSays.Stop();
        Log.Info("BOMB EXPLODE");
    }

    public void BombSolved()
    {
        Solved = true;
        Arduino.Solve();
        soundplayer.Play(Path.Combine("Sounds", "bomb_defused.wav"));
        simonSays.Stop();
        Log.Info("BOMB SOLVED");
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
        sb.AppendLine($"   [     Bomb] MaxMistakes: {MaxMistakes}, Mistakes: {Mistakes}, Time: {initialTime}");
        sb.AppendLine($"   [    Frame] {Frame}");

        foreach (IModule module in modules)
        {
            if (module.IsActive)
                sb.AppendLine($"   [{module.GetType().Name,9}] {module}");
        }

        return sb.ToString();
    }
}
