using System;

namespace Wordle
{
    public static class Wordle
    {
        public static Random random;
        private static Board board;
        public static void Swap<T>(ref T a, ref T b)
        {
            T temp = a;
            a = b;
            b = temp;
        }

        public static void PrintHelp()
        {
            Console.WriteLine(" -a answer\t Answer to the board.");
            Console.WriteLine(" -d file  \t Location to a dictionary file in plain text form.");
            Console.WriteLine(" -t num   \t Number of rounds. (default=5)");
        }

        public static string GetValidInput(Board brd, Dictionary dict)
        {
            while(true)
            {
                string input = GetSanitizedInput(brd);
                if (dict.Contains(input)) return input;
                Console.WriteLine("Enter a valid english word.");
            }
        }

        public static string GetSanitizedInput(Board brd)
        {
            while (true)
            {
                Console.Write($"Enter a {brd.WordLen} letter word to try: ");
                var input = Console.ReadLine();
                if (input.Length != brd.WordLen) { Console.WriteLine($"The word entered was not {brd.WordLen} letters long!"); continue; }

                bool invalid = false;
                string output = "";
                for(int i = 0; i < input.Length; ++i)
                {
                    char c = input[i];
                    if (char.IsUpper(c)) c = char.ToLower(c);
                    if(!char.IsLower(c))
                    {
                        Console.WriteLine("Please enter only letters!");
                        invalid = true;
                        break;
                    }
                    output += c;
                }
                if (!invalid) return output;
            }
        }

        public static int Main(string[] args)
        {
            random = new();
            Console.WriteLine("Wordle clone by Adam Warren \u00a9 2022");
            Console.CancelKeyPress += Console_Sigint;

            Console.WriteLine(System.IO.Directory.GetCurrentDirectory().ToLower());

            Dictionary list = new("engmix.txt");
            board = new Board(6, list, 5, 5);

            board.Print();

            var input = GetValidInput(board, list);
            Console.WriteLine($"Entered the word \"{input}\"");

            int res;
            while((res = board.InsertGuess(input)) == 0)
            {
                board.Print();
                input = GetValidInput(board, list);
            }

            board.Print();

            if(res == 2)
            {
                Console.WriteLine($"Better luck next time, the answer was \"{board.Answer}\"");
            }

            return 0;
        }

        private static void Console_Sigint(object sender, ConsoleCancelEventArgs e)
        {
            if (board != null) Console.WriteLine($"The answer was \"{board.Answer}\"");
            Environment.Exit(0);
        }
    }
}
