#include <iostream>
#include <unordered_map>  //map
#include <string> //string
using namespace std;
class solution {
  public:  int lengthOfLongestSubstring(string s)
    {
        unordered_map<char,int>windows;
        int left=0;
        int length=s.length();
        int max_length=0;
        for(int i=0;i<s.length();i++)
        {
            windows[s[i]]+=1;
            // 左窗口收缩，只有右边届的引入，才可能导致字符重复
            while(windows[s[i]]==2)
            {
                windows[s[left]]-=1;
                left++;
            }
           max_length=max(max_length,i-left+1);
        }
        return max_length;
    }
};
int main()
{
    
    string s1="abcabcbb";
    string s2="pwwkew";
    string s3="bbbbbbb";
    solution s;
   int a= s.lengthOfLongestSubstring(s1);
   int b= s.lengthOfLongestSubstring(s2);
   int c= s.lengthOfLongestSubstring(s3);
   cout<<"a="<<a<<endl;
   cout<<"b="<<b<<endl;
   cout<<"c="<<c<<endl;
}