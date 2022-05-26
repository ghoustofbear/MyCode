
from collections import defaultdict
class Solution:
    def findRepeatedDnaSequences(self, s: str) -> List[str]:
        arr=[]
        L=10
        if(len(s)<=L):
            return []
        s1=defaultdict(int)
        for i in range(0,len(s)-L+1):
            s1[s[i:i+L]]+=1
            if(s1[s[i:i+L]]==2):
                arr.append(s[i:i+L])
        return arr
                

def main():
    k=Solution()
    s = "AAAAACCCCCAAAAACCCCCCAAAAAGGGTTT"
    t=k.findRepeatedDnaSequences(s)
    print("t="+str(t))
if __name__=="__main__":
    main()
                
            
            
            
            