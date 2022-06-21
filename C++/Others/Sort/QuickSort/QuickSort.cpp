#include<chrono>
#include<random>
#include<iostream>
#include<vector>
#include <time.h>//引入头文件

void QuickSorrt(std::vector<int>&nums,int left,int right)
{
    if(left<right)
    {
    int l=left;
    int r=right;
    int priot=left;
    int x=nums[priot];
    while(l<r)
    {
        while(l<r&&nums[r]>=x)
        {
            r--;
        }
        nums[l]=nums[r];
        while(l<r&&nums[l]<=x)
        {
            l++;
        }
        nums[r]=nums[l];
    }
    nums[l]=x;
    int mid=l;
    QuickSorrt(nums,left,mid-1);
    QuickSorrt(nums,mid+1,right);
    }

}
void Quick_Sort_II(std::vector<int>&nums,int left,int right)
{
    if(left<right)
    {

    int l=left;
    int r=right;
    //int prior=left+(right-left)/2;
    int prior=left;
    int x=nums[prior];
    while(l<r)
    {
        while(l<r&&nums[r]>=x)
        {
            r--;
        }
        nums[l]=nums[r];
        while(l<r&&nums[l]<=x)
        {
            l++;
        }
        nums[r]=nums[l];

    }
    nums[l]=x;
    int mid=l;
    Quick_Sort_II(nums,mid+1,right);
    Quick_Sort_II(nums,left,mid-1);

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
    //std::vector<int>test{23, 64, 24, 12, 9, 16, 53, 57, 71, 79, 87, 97};
    std::vector<int> test;
    createRand(test,8000);
    
    clock_t start,end;//定义clock_t变量
    start = clock(); //开始时间
    Quick_Sort_II(test,0,test.size()-1);
    end = clock();   //结束时间
    std::cout<<"test快排的时间 = "<<double(end-start)/CLOCKS_PER_SEC<<"s"<<std::endl;  //输出时间（单位：ｓ）
    for (auto m:test)
    {
        std::cout<<"快排后m="<<m<<std::endl;
    }

    return 0;
}
