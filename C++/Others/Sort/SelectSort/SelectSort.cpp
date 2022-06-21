#include<iostream>
#include<vector>
void SelectSort(std::vector<int>&nums)
{
    
int length=nums.size();
 for(int i=0;i<length;i++)
 {
   for(int j=i+1;j<length;j++)
   {
     if(nums[i]>nums[j])
     { 
        int temp=nums[i];
        nums[i]=nums[j];
        nums[j]=temp;
     }
   } 
 }
}
void createRand(std::vector<int>&nums,int N)
{
    for(int i=0;i<N;i++)
    {
        nums.emplace_back(rand()%N);
        std::cout<<"随机数nums[i]="<<nums[i]<<std::endl;
    }

}
int main(int argc, char const *argv[])
{
    /* code */
    std::vector<int> test;
    createRand(test,5000);
    SelectSort(test);
    for(auto &i:test)
    {
        std::cout<<"i="<<i<<std::endl;
    }
    return 0;
}
