#### [219. 存在重复元素 II](https://leetcode-cn.com/problems/contains-duplicate-ii/)

难度简单461

给你一个整数数组 `nums` 和一个整数 `k` ，判断数组中是否存在两个 **不同的索引** `i` 和 `j` ，满足 `nums[i] == nums[j]` 且 `abs(i - j) <= k` 。如果存在，返回 `true` ；否则，返回 `false` 。

 

**示例 1：**

```
输入：nums = [1,2,3,1], k = 3
输出：true
```

**示例 2：**

```
输入：nums = [1,0,1,1], k = 1
输出：true
```

**示例 3：**

```
输入：nums = [1,2,3,1,2,3], k = 2
输出：false
```

#### 解法及其思路：

本题核心在于abs(i - j) <= k，set来控制是否相等条件，控制滑动窗口长度，来实现滑动

如果窗口中有重复值，就可以直接返回True，遍历结束没找到存在重复值的窗口，返回False

```python
  
                              
class Solution:
    def containsNearbyDuplicate(self, nums: list[int], k: int) -> bool:
        s = set()
        for i, num in enumerate(nums):
            if i > k:
                s.remove(nums[i - k - 1])
            if num in s:
                return True
            s.add(num)
        return False

                 
                
       
def main():
    m=Solution()
    nums = [1,2,3,1]
    k = 3
    nums1 = [1,0,1,1]
    k1= 1
    nums2=[1,2,3,1,2,3]
    k2=2
 
    t=m.containsNearbyDuplicate(nums2,k2)
    print("t="+str(t))
if __name__=="__main__":
    main()
```

