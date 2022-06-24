#include<iostream>
#include<vector>
int divide(std::vector<int>&nums,int left,int right)
{
  
    
        int l=left;
        int r=right;
        int x=nums[l+(r-l)/2];
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
        return l;
    
}
int findKMax(std::vector<int>&nums,int left,int right,int k)
{
    if(left<right)
    {
        int mid=divide(nums,left,right);
        int length_of_right=right-mid+1;
        if(length_of_right==k)
        {
          return nums[mid];
        }
        else if(length_of_right>k)
        {
        //右半边长度比k长，说明第k大的元素还在右半边，因此在右半边找
         return findKMax(nums,mid+1,right,k);
        }
        else
        {
        
          return findKMax(nums,left,mid-1,k);
        }
    }
    else
    {
        return ;
    }
    
}
void QuickSort(std::vector<int>&nums,int left,int right)
{
    if(left<right)
    {
        int mid=divide(nums,left,right);
        QuickSort(nums,mid+1,right);
        QuickSort(nums,left,mid-1);
    }
    else
    {
        return ; 
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
    std::vector<int>test;
    std::vector<int>test1{6,5,4,3,2,1,0 };
    createRand(test,1000);
    int n=test.size();
    int n1=test1.size();
     int t=findKMax(test,0,n-1,3);
    //QuickSort(test,0,test.size()-1);
    for(auto&i:test)
    {
        std::cout<<"test[i]="<<i<<std::endl;
    }
    std::cout<<"t="<<t<<std::endl;
    return 0;
}
