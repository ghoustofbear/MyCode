from codecs import lookup


class Solution:
    def lengthOfLongestSubstring(self, s: str) -> int:
        length=len(s)
        current_length=0
        max_length=0
        lookup=set()
        left=0
        for i in range(length):
            current_length+=1
            while s[i] in lookup:
                lookup.remove(s[left])
                current_length-=1
                left+=1
            if current_length > max_length:max_length = current_length
           
            lookup.add(s[i])
        return max_length
def main():
    k=Solution()
    s="abcabcbb"
    t=k.lengthOfLongestSubstring(s)
    print("t="+str(t))
if __name__=="__main__":
    main()
                
       