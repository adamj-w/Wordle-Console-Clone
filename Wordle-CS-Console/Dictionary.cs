using System;
using System.Collections.Generic;
using System.IO;

namespace Wordle
{
    class DictionaryComparer : IComparer<string>
    {
        public int Compare(string x, string y)
        {
            return x.ToLower().CompareTo(y.ToLower());
        }
    }

    public class Dictionary
    {
        private List<string> dictionary;
        private bool alphabetized = false;
        private static DictionaryComparer comparer = new();

        public int WordCount { get { return dictionary.Count; } }

        public Dictionary(string filepath)
        {
            if (!File.Exists(filepath)) throw new FileNotFoundException(filepath);
            dictionary = new List<string>(File.ReadAllLines(filepath));
        }

        public bool Contains(string str)
        {
            return IndexOf(str) >= 0;
        }

        public int IndexOf(string str)
        {
            if (!alphabetized) alphabetize();

            return dictionary.BinarySearch(str, comparer);
        }

        public string GetWord(int i)
        {
            return dictionary[i];
        }

        public void PrintSublist(int offset, int size)
        {
            if (offset + size >= dictionary.Count) size = dictionary.Count - offset - 1;

            foreach (var item in dictionary.GetRange(offset, size))
                Console.WriteLine(item);
        }

        public void SanitizeToLower()
        {
            dictionary.RemoveAll((x) => {
                return Array.FindAll(x.ToCharArray(), (c) => !char.IsLower(c)).Length > 0;
            });
        }

        public void SanitizeToLength(int len)
        {
            dictionary.RemoveAll((x) => x.Length != len);
        }

        private void alphabetize()
        {
            dictionary.Sort(comparer);
            alphabetized = true;
        }

        public Dictionary Clone()
        {
            return (Dictionary)MemberwiseClone();
        }
    }
}
