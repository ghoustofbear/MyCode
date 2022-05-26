#include<iostream>
#include<vector>
class Solution {
public:
    int minSubArrayLen(int target, std::vector<int>& nums) {
        int n=nums.size();
        int sum=0;
        int min_length=INT32_MAX;
        int left=0;
        for (int i = 0; i < n; i++)
            {
                /* code */
                sum+=nums[i];
                while(sum>=target)
                {
                   
                       min_length=i-left+1<min_length? i-left+1:min_length;
                       sum-=nums[left];
                        left+=1;              
                }      
            }        
          return min_length==INT32_MAX ? 0:min_length;  
    }
    
};
int main(int argc, char const *argv[])
{
   int target = 4;
    std::vector<int>nums{1,4,4};
    Solution I;
    int &&t=I.minSubArrayLen(target,nums);
    std::cerr<<"t="<<t<<std::endl;
}