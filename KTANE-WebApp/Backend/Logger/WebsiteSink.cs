using Serilog;
using Serilog.Configuration;
using Serilog.Core;
using Serilog.Events;

namespace KTANE.Backend.Logger;

public static class WebsiteSinkExtensions
{
    public static LoggerConfiguration WebsiteSink(
        this LoggerSinkConfiguration loggerConfiguration,
        IFormatProvider? formatProvider = null)
    {
        return loggerConfiguration.Sink(new WebsiteSink(formatProvider));
    }
}

public class WebsiteSink : ILogEventSink
{
    private readonly IFormatProvider? formatProvider;

    public static string Text { get; private set; } = string.Empty;

    public static void ClearText() => Text = string.Empty;

    public WebsiteSink(IFormatProvider? formatProvider)
    {
        this.formatProvider = formatProvider;
    }

    public void Emit(LogEvent logEvent)
    {
        // (Console.ForegroundColor, Console.BackgroundColor) = logEvent.Level switch
        // {
        //     LogEventLevel.Verbose => (ConsoleColor.DarkGray, defaultBackground),
        //     LogEventLevel.Debug => (ConsoleColor.DarkGray, defaultBackground),
        //     LogEventLevel.Information => (ConsoleColor.Gray, defaultBackground),
        //     LogEventLevel.Warning => (ConsoleColor.Yellow, defaultBackground),
        //     LogEventLevel.Error => (ConsoleColor.Red, defaultBackground),
        //     LogEventLevel.Fatal => (ConsoleColor.Red, ConsoleColor.Yellow),
        //     _ => (defaultForeground, defaultBackground)
        // };

        Text += logEvent.RenderMessage(formatProvider) + Environment.NewLine;
    }
}
