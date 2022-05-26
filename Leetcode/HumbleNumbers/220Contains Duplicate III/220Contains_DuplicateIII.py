'''
Author: liuxu
Date: 2022-03-29 11:23:59
LastEditors: liuxu
LastEditTime: 2022-04-07 11:11:07
FilePath: \Mycodes\Leetcode\HumbleNumbers\220Contains Duplicate III\220Contains_DuplicateIII.py
Description: 

Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
'''


# class Solution:
#     def containsNearbyAlmostDuplicate(self, nums: list[int], k: int, t: int) -> bool:
#         s=set()
#         left=0
#         for i ,num in enumerate(nums):
#             if i-left>k:
#                 s.remove(nums[left])
#                 left+=1
#             if()
            
     
from sortedcontainers import SortedList
import _bisect

class Solution:
    def containsNearbyAlmostDuplicate(self, nums: list[int], k: int, t: int) -> bool:
        
        window = SortedList()
        sz = len(nums)
        left = 0
        right = 0

        if k == 0 or sz in (0, 1):
            return False
            
        while right < sz:
            window.add(nums[right])
            if right == 0: # 如果是第一个就跳过
                right += 1
                continue
            idx = window.bisect_left(nums[right]) # 在窗口内的排序
            if idx > 0 and abs(window[idx] - window[idx-1]) <= t:
                # 如果待测值在窗口内不排在第一位（最小值），向左找
                # 并且待测值在窗口内的排序的前一位，是小于等于待测值的最大值，
                # 取这两个值的绝对值于 t 相比较
                # 因为如果最相近的值都不满足 小于等于t 的条件，那么其他值也不能可能满足
                return True
            if idx < len(window) - 1 and abs(window[idx+1] - window[idx]) <= t:
                # 如果待测值在窗口内不排在最后一位（最大值），向左找
                return True
            
            if right >= k:
                window.remove(nums[left])
                left += 1
            
            right += 1
        return False

    def containsNearbyAlmostDuplicateIII(self, nums: list[int], k: int, t: int) -> bool:
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


    def containsNearbyAlmostDuplicateII(self, nums: list[int], k: int, t: int) -> bool:
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
        
def main():
    nums=[1,0,1,1]
    k=1
    t=2
    nums1 = [1,2,3,1] 
    k1 = 3 
    t1 = 0
    nums2=[1,5,9,1,5,9]
    k2=2
    k2=3
    print(Solution().containsNearbyAlmostDuplicate(nums2,k2,k2))
    print(Solution().containsNearbyAlmostDuplicateII(nums2,k2,k2))
    print(Solution().containsNearbyAlmostDuplicateIII(nums2,k2,k2))
if __name__ == '__main__':
    main()