
from collections import defaultdict
class Solution:
     def minWindow(self, s: str, t: str) -> str:
        target=defaultdict(int)
        for c in t:
            target[c]+=1
        min_len=float('inf')
        counter=len(t)
        left=0
        right=0
        result=""
        while right<len(s):
            if target[s[right]]>0:
                counter-=1
            target[s[right]]-=1
            right+=1
            while counter==0:
                if min_len>right-left:
                    min_len=right-left
                    result=s[left:right]
                if target[s[left]]==0:
                    counter+=1
                target[s[left]]+=1
                left+=1     
        return result         
def main():
        s = "TMFRAFDOBECODEBANC"
        t = "ABC"
        
        s1="a"
        t1="aa"
        m=Solution()
        print(m.minWindow(s1,t1))
        
if __name__=='__main__':
    main()
        
                        
                    
                    
                    
            
        
            
        
