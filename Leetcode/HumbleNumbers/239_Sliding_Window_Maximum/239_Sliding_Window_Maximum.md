#### [239. 滑动窗口最大值](https://leetcode-cn.com/problems/sliding-window-maximum/)

难度困难1534

给你一个整数数组 `nums`，有一个大小为 `k` 的滑动窗口从数组的最左侧移动到数组的最右侧。你只可以看到在滑动窗口内的 `k` 个数字。滑动窗口每次只向右移动一位。

返回 *滑动窗口中的最大值* 。

 

**示例 1：**

```
输入：nums = [1,3,-1,-3,5,3,6,7], k = 3
输出：[3,3,5,5,6,7]
解释：
滑动窗口的位置                最大值
---------------               -----
[1  3  -1] -3  5  3  6  7       3
 1 [3  -1  -3] 5  3  6  7       3
 1  3 [-1  -3  5] 3  6  7       5
 1  3  -1 [-3  5  3] 6  7       5
 1  3  -1  -3 [5  3  6] 7       6
 1  3  -1  -3  5 [3  6  7]      7
```

**示例 2：**

```
输入：nums = [1], k = 1
输出：[1]
```

 

**提示：**

- `1 <= nums.length <= 105`
- `-104 <= nums[i] <= 104`
- `1 <= k <= nums.length`

通过次数269,423

提交次数540,536

#### 解题思路及方法：

##### 解放一：滑动窗口：

```python
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
def main():
    nums = [1,3,-1,-3,5,3,6,7]
    k = 3      
    m=Solution()
    t= m.maxSlidingWindow(nums,k)
    
    print(t)  
if __name__=='__main__':
    main()    
            
```

##### 解放二：单调队列

```python
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

```

