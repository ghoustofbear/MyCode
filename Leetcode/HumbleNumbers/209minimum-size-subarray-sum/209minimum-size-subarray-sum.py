class Solution:
    def minSubArrayLen(self, target: int, nums: list[int]) -> int:
        sum=0
        left=0
        current_length=0
        max_length=0
        arr=[]
        for i in range(0,len(nums)):
            sum+=nums[i];
            current_length+=1
            while sum>=target:
                left+=1
                current_length-=1
                sum-=nums[i]
            if current_length>max_length:max_length=current_length
        return left
def main():
    k=Solution()
    target = 7
    nums = [2,3,1,2,4,3]
    t=k.minSubArrayLen(target,nums)
    print("t="+str(t))
if __name__=="__main__":
    main()
                
                
                
            