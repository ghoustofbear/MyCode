#include<iostream>
#include<unordered_map>
#include<vector>
using namespace std;
class Solution {
public:
    std::vector<std::string> findRepeatedDnaSequences(std::string s) {
        std::unordered_map<std::string,int>words;
        std::vector<std::string> result;
        int left=0;
        int n=s.size();
        for(int i=0;i<=n-10;i++)
        {
            std::string m=s.substr(i,10);
            words[m]++;
            if(words[m]==2)
            {
                result.emplace_back(m);
            }
        }
        return result;
    }
};

int main(int argc,char const *argv[])
{
    
    std::string n="AAAAACCCCCAAAAACCCCCCAAAAAGGGTTT";
    Solution s;

     auto result=s.findRepeatedDnaSequences(n);
    
    for(auto it:result)
    {
        std::cerr<<"it="<<it<<std::endl;
    }

    return 0;
}