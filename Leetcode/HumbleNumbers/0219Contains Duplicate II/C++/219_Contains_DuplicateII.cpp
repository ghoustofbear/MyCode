#include<iostream>
#include<vector>
#include<map>
class Solution {
public:
    bool containsNearbyDuplicate(std::vector<int>& nums, int k) {
        int n=nums.size();
        bool ret=false;
        int left=0;
        std::map<int,int>windows;
        for(int i=0;i<n;i++)
        {
            windows[nums[i]]++;
            if(i-left>k)
            {
                windows[nums[left]]--;
                left++;
            }
            else
            {
                if(windows[nums[i]]==2)
                {
                    ret=true;
                    std::cerr<<"left="<<left<<std::endl;
                    std::cerr<<"i="<<i<<std::endl;
                }
            }
        }
        return ret;
    }
};
int main(int argc ,char*argv[])
{
    std::vector<int>nums {1,0,1,1} ;
    int k=1;
    
    Solution s;
    auto m=s.containsNearbyDuplicate(nums,k);
    std::cerr<<"m="<<m<<std::endl;
    return 0;
}