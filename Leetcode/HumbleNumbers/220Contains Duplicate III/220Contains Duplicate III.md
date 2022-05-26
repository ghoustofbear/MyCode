#### [220. 存在重复元素 III](https://leetcode-cn.com/problems/contains-duplicate-iii/)



给你一个整数数组 `nums` 和两个整数 `k` 和 `t` 。请你判断是否存在 **两个不同下标** `i` 和 `j`，使得 `abs(nums[i] - nums[j]) <= t` ，同时又满足 `abs(i - j) <= k` 。

如果存在则返回 `true`，不存在返回 `false`。

 

**示例 1：**

```
输入：nums = [1,2,3,1], k = 3, t = 0
输出：true
```

**示例 2：**

```
输入：nums = [1,0,1,1], k = 1, t = 2
输出：true
```

**示例 3：**

```
输入：nums = [1,5,9,1,5,9], k = 2, t = 3
输出：false
```

 

**提示：**

- `0 <= nums.length <= 2 * 104`
- `-231 <= nums[i] <= 231 - 1`
- `0 <= k <= 104`
- `0 <= t <= 231 - 1

#### 解放及其思路：

##### 解放一：滑动窗口+二分法

题目有两个不确定的因素：

两个不同位置下标的差的绝对值小于等于 k；
两个不同位置的数值的差的绝对值小于等于 t。
暴力解法同时考虑了两个因素，在遍历数组的过程中没有记录下有用的信息。相比于两个不同位置的数值，下标的值是容易掌握的。于是我们可以 维护长度为 k+1 的滑动窗口，把滑动窗口滑过的元素加入一个数据结构，把滑动窗口滑出的元素移出数据结构。（滑动窗口的长度是 k + 1，但是真正数据结构里保存的元素个数可能是 k 也可能是 k + 1，取决于先插入新元素，还是先移除旧元素，这里不展开叙述了。）

我们把题目意思翻译一下：在数组 nums[i] 中，在任意长度大于等于 2 的区间 [i..i + k] 里是否存在两个数的绝对值小于等于 t，即

|nums[i] - nums[j] | <= t

去掉绝对值符号

 tnums[i]−nums[j]<=t （不等式 1）

并且

nums[i] - nums[j] >=−t （不等式 2）。

可以把 nums[i] 看作新遍历到的元素，把 nums[j] 看作当前在滑动窗口中的元素。

根据（不等式 1）得：


nums[i]−t≤nums[j]

根据（不等式 2）得：


nums[i]+t≥nums[j]

由此我们可以得到：如果在滑动窗口中能够找到一个元素 nums[j]，它的值大于等于nums[i]−t（条件 1），并且还要小于等于 nums[i]+t（条件 2）。

滑动窗口中可以有很多个元素的值满足「条件 1」，为了能够找同时满足两个条件的合适的 nums[j]，由于「条件 2」中 nums[i]−t 的值是固定的，为了让「条件 2」有更多的可能性成立，「条件 1」找到的 nums[j] 需要尽可能小，这件事情等价于找到大于等于 nums[i]−t 的最小的值，即找到 nums[i]−t 的最小上界。

```python
class Solution:
     def containsNearbyAlmostDuplicate(self, nums: list[int], k: int, t: int) -> bool:
        from sortedcontainers import SortedList
        if k<1 or t<0:  
            return False
        windows=SortedList()
        left=0
        for i,values in enumerate(nums):
            if i-left>k:
                windows.remove(nums[left])
                left+=1
            
            L=windows.bisect_left(values-t)
            if  L>=0 and L<len(windows) and windows[L]<=(t+values):
                return True
            windows.add(values)
        return False  
```



##### 解法二：二分法

```python
class Solution:
     def containsNearbyAlmostDuplicate(self, nums: list[int], k: int, t: int) -> bool:
            ##########  二分
        if t < 0:
            return False

        window = []     #滑动窗口

        for i,num in enumerate(nums):
            idx = _bisect.bisect_left(window,num - t)   #第一个 >= (num-t)的index
            if idx < len(window) and window[idx] <= (num + t):
                return True

            idx = _bisect.bisect_left(window,num)
            window.insert(idx, num)             #进R

            if i >= k:                          #弹L
                window.remove(nums[i-k])

        return False
```

