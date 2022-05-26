#include<iostream>
#include<unordered_map>
using namespace std;
class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        std::string result;
        std::unordered_map<char,int>words,windows;
        int left=0;
        
        for(auto& word:t)
        {
            words[word]+=1;
        }
        int count=0;
        for( int i=0;i<s.size();i++)
        { 
            
            windows[s[i]]++;
            if(windows[s[i]]==words[s[i]])
            {
                count+=1;
            }
            while(windows[s[left]]>words[s[left]])
            {
                
                windows[s[left++]]--;
            }
            if(count==t.size())
            {
                auto flag=result.empty()||i-left+1<result.size();
                if(result.empty()||i-left+1<result.size())
                {
                    result=s.substr(left,i-left+1);
                    
                }
            }
        }
        return result;
    }
    



};
int main(int argc, char const *argv[])
{
    /* code */
    std::string m="ADOBECODEBANC";
    std::string n="ABC";
    Solution s;
    std::string &&t=s.minWindow(m,n);
    
    std::cerr<<"result= "<<t<<std::endl;
   
    return 0;
}

