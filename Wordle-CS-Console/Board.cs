using System;
using System.Collections.Generic;

namespace Wordle
{
    public class Board
    {
        public static readonly string[] formats = { "\x1b[0m", "\x1b[30;1m", "\x1b[33;1m", "\x1b[32;1m" };
        public enum Fmt 
        {
            Reset = 0, Grey = 1, Yellow = 2, Green = 3
        }

        public string Answer { get; }
        public int Attempts { get; }
        public int WordLen { get; }

        private int currentRow = 0;
        private List<Tuple<char, Fmt>> board;

        public Board(int trys, Dictionary dict, int minWordLen, int maxWordLen)
        {
            Attempts = trys;
            if (minWordLen > maxWordLen) Wordle.Swap(ref minWordLen, ref maxWordLen);
            int len = Wordle.random.Next(minWordLen, maxWordLen);
            WordLen = len;

            board = new();
            for(int i = 0; i < len * Attempts; i++)
                board.Add(new Tuple<char, Fmt>(' ', Fmt.Reset));

            Dictionary copy = dict.Clone();
            copy.SanitizeToLength(len);
            Answer = copy.GetWord(Wordle.random.Next(0, copy.WordCount));
        }

        public void Print()
        {
            for(int i = 0; i < Attempts; ++i)
            {
                for (int j = 0; j < WordLen; ++j)
                {
                    Console.Write("----");
                }
                Console.WriteLine();

                for(int j = 0; j < WordLen; j++)
                {
                    var pair = board[i * WordLen + j];
                    Console.Write($"| {formats[(int)pair.Item2]}{pair.Item1}{formats[0]} ");
                }

                Console.WriteLine('|');
            }

            for (int j = 0; j < WordLen; ++j)
            {
                Console.Write("----");
            }
            Console.WriteLine();

#if DEBUG
            Console.WriteLine($"Answer is \"{Answer}\", current row: {currentRow}");
#endif
        }

        public int InsertGuess(string guess)
        {
            for(int i = 0; i < WordLen; ++i)
            {
                Fmt fmt = Fmt.Grey;
                if(Answer[i] == guess[i])
                {
                    fmt = Fmt.Green;
                } 
                else if(Array.FindIndex(Answer.ToCharArray(), (c) => c == guess[i]) > 0)
                {
                    fmt = Fmt.Yellow;
                }
                board[currentRow * WordLen + i] = new Tuple<char, Fmt>(guess[i], fmt);
            }

            if(Answer.CompareTo(guess) == 0)
            {
                Console.WriteLine("You Win!!");
                return 1;
            }
            if(++currentRow >= Attempts)
            {
                Console.WriteLine("You lost!");
                return 2;
            }
            return 0;
        }
    }
}
