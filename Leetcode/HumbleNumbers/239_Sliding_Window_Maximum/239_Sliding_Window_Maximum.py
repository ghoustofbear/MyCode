
from bisect import bisect
from sortedcontainers import SortedList, SortedSet


class Solution:
    def maxSlidingWindow(self, nums: list[int], k: int) -> list[int]:
        left=0
        windows=SortedList()
        
        target=[]
        for i,values in enumerate(nums):
            
            
            if i-left>k-1:
                target.insert(left,windows[len(windows)-1])
                windows.remove(nums[left])
                left+=1
            windows.add(values)
        target.insert(left,windows[len(windows)-1])
        
        return target
    def maxSlidingWindowII(self, nums: list[int], k: int) -> list[int]:

        # 如果数组为空或 k = 0，直接返回空
        if not nums or not k:
            return []
        # 如果数组只有1个元素，直接返回该元素
        if len(nums) == 1:
            return [nums[0]]

        # 初始化队列和结果，队列存储数组的下标
        queue = []
        res = []

        for i in range(len(nums)):
            # 如果当前队列最左侧存储的下标等于 i-k 的值，代表目前队列已满。
            # 但是新元素需要进来，所以列表最左侧的下标出队列
            if queue and queue[0] == i - k:
                queue.pop(0)

            # 对于新进入的元素，如果队列前面的数比它小，那么前面的都出队列
            while queue and nums[queue[-1]] < nums[i]:
                queue.pop()
            # 新元素入队列
            queue.append(i)

            # 当前的大值加入到结果数组中
            if i >= k-1:
                res.append(nums[queue[0]])

        return res


def main():
    nums = [1,3,-1,-3,5,3,6,7]
    k = 3      
    m=Solution()
    t= m.maxSlidingWindow(nums,k)
    
    print(t)  
if __name__=='__main__':
    main()    
            
            
