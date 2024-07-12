namespace KTANE_WebApp.Backend.Module;

public class Password : IModule
{
    private readonly string[] possibleWords =
    [
        "ANGST", "ATMEN", "BETEN", "BOMBE", "DANKE",
        "DRAHT", "DRUCK", "DRÜCK", "FARBE", "FEHLT",
        "FERSE", "KABEL", "KNALL", "KNAPP", "KNOPF",
        "LEERE", "LEGAL", "LEHRE", "MATHE", "MATTE",
        "PANIK", "PIEPS", "RAUCH", "RUHIG", "SAITE",
        "SEHNE", "SEITE", "SENDE", "STROM", "SUPER",
        "TIMER", "ÜBRIG", "VERSE", "WARTE", "ZANGE"
    ];

    private readonly char[] alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÜ".ToCharArray();

    public bool IsSolved { get; private set; }

    private string correctWord = string.Empty;
    private List<List<char>> possibleCharacters = [];
    private readonly int[] selectedCharacters = new int[5];

    public void Generate(Random random)
    {
        correctWord = possibleWords[random.Next(possibleWords.Length)];
        possibleCharacters = GetCharacterValues(random);
        for (int i = 0; i < selectedCharacters.Length; i++)
        {
            selectedCharacters[i] = random.Next(possibleCharacters[i].Count);
        }
    }

    public char[] GetCurrentWord()
    {
        char[] word = new char[selectedCharacters.Length];
        for (int i = 0; i < selectedCharacters.Length; i++)
        {
            word[i] = possibleCharacters[i][selectedCharacters[i]];
        }

        return word;
    }

    public void HandelButtonPress(Bomb bomb, bool send, bool isUp, int characterIndex)
    {
        if (send)
        {
            if (IsSelectedWordCorrect())
            {
                IsSolved = true;
                bomb.UpdateSolvedModules();
                Arduino.SetPasswordText([(char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF, (char)0xFF]);
                return;
            }

            bomb.IncrementTries();
            return;
        }

        ChangeSelectedCharacter(isUp, characterIndex);
        Arduino.SetPasswordText(GetCurrentWord());
    }

    private bool IsSelectedWordCorrect()
    {
        for (int i = 0; i < selectedCharacters.Length; i++)
        {
            if (correctWord[i] != possibleCharacters[i][selectedCharacters[i]])
            {
                return false;
            }
        }

        return true;
    }

    private void ChangeSelectedCharacter(bool isUp, int position)
    {
        int newPos = selectedCharacters[position] + (isUp ? 1 : -1);
        int maxPos = possibleCharacters[position].Count;
        selectedCharacters[position] = (newPos % maxPos + maxPos) % maxPos; // Modulo without negative numbers
    }

    // Copied from KTANE
    private List<List<char>> GetCharacterValues(Random random)
    {
        List<List<char>> charValueList = [];
        AddValues(charValueList);
        PruneFalseMatches(charValueList);
        ReduceToCharacterCount(charValueList);
        return charValueList;

        void ReduceToCharacterCount(List<List<char>> charValues)
        {
            for (int i = 0; i < charValues.Count; i++)
            {
                char c = correctWord[i];
                charValues[i].Remove(c);
                while (charValues[i].Count > 5)
                {
                    charValues[i].Remove(charValues[i][random.Next(0, charValues[i].Count)]);
                }

                charValues[i].Add(c);
                charValues[i].Shuffle(random);
            }
        }

        void AddValues(List<List<char>> charValues)
        {
            for (int i = 0; i < 5; i++)
            {
                List<char> list = [];
                charValues.Add(list);
                list.AddRange(alphabet);
            }
        }

        void PruneFalseMatches(List<List<char>> charValues)
        {
            List<string> list = GetFalseMatches(charValues);
            while (list.Count > 0)
            {
                string text = list[0];
                List<int> incorrectCharPositions = GetIncorrectCharPositions(text);
                if (incorrectCharPositions.Count > 0)
                {
                    int num = incorrectCharPositions[random.Next(0, incorrectCharPositions.Count)];
                    char c = text[num];
                    charValues[num].Remove(c);
                }

                list = GetFalseMatches(charValues);
            }
        }

        List<int> GetIncorrectCharPositions(string word)
        {
            List<int> list = [];
            for (int i = 0; i < word.Length; i++)
            {
                if (word[i] != correctWord[i])
                {
                    list.Add(i);
                }
            }

            return list;
        }

        List<string> GetFalseMatches(List<List<char>> charValues)
        {
            List<string> matches = GetMatches(charValues);
            if (matches.Contains(correctWord))
            {
                matches.Remove(correctWord);
            }

            return matches;
        }

        List<string> GetMatches(List<List<char>> charValues)
        {
            List<string> list = [];
            foreach (string text in possibleWords)
            {
                char[] array = text.ToCharArray();
                bool flag = true;
                for (int i = 0; i < charValues.Count; i++)
                {
                    bool flag2 = false;
                    for (int j = 0; j < charValues[i].Count; j++)
                    {
                        if (charValues[i][j] == array[i] || charValues[i].Count == 1)
                        {
                            flag2 = true;
                            break;
                        }
                    }

                    if (!flag2)
                    {
                        flag = false;
                        break;
                    }
                }

                if (flag)
                {
                    list.Add(text);
                }
            }

            return list;
        }

        List<char> GetCharactersInPosition(List<string> matches, int position)
        {
            List<char> list = [];
            foreach (string text in matches)
            {
                char c = text.ToCharArray()[position];
                if (!list.Contains(c))
                {
                    list.Add(c);
                }
            }

            return list;
        }
    }

    public void Reset()
    {
        IsSolved = false;
    }

    public override string ToString()
    {
        return $"Solved: {IsSolved}, CorrectWord {correctWord}, DisplayedWord: {new string(GetCurrentWord())}";
    }
}
