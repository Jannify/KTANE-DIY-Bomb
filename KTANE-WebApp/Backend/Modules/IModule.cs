namespace KTANE.Backend.Modules;

public interface IModule
{
    bool IsActive { get; set; }
    bool IsSolved { get; }

    void Reset();
}
