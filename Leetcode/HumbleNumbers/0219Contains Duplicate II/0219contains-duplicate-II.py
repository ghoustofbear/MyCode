
                              
import bisect
from collections import defaultdict
from pickle import FALSE
from turtle import update
from typing import Dict


class Solution:
    def containsNearbyDuplicate(self, nums: list[int], k: int) -> bool:
        s = set()
        left=0
        for i, num in enumerate(nums):
            if i-left > k:
                s.remove(nums[left])
                left+=1
            if num in s:
                print("i=%d",i)
                return True
            s.add(num)
        return False
    def containsNearbyDuplicateII(self, nums: list[int], k: int) -> bool:
        import bisect
        windows=[]
        for i,values in enumerate(nums):
            if values in windows:
                return True
            index=bisect.bisect_left(windows,values)
            windows.insert(index,values)
            if i-k>=0:
                windows.remove(nums[i-k])
        return False
            
        
        
   
                            
                 
                
       
def main():
    m=Solution()
    nums = [1,2,3,1]
    k = 3
    nums1 = [1,0,1,1]
    k1= 1
    nums2=[1,2,3,1,2,3]
    k2=2
 
    t=m.containsNearbyDuplicate(nums1,k1)
    print("t="+str(t))
if __name__=="__main__":
    main()
    
        
            
                    
                    
                
                
            
            
    