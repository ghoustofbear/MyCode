class Solution:
    def climbStairs(self,n:int) ->int:
        a=1
        b=2
        ans=0
        if(n<=2):
            return n
        for i in range(3,n+1):
            ans=a+b
            a,b=b,ans
        return ans   
def main():
    n=Solution()
    print("n="+str(n.climbStairs(3)))
if __name__=="__main__":
    main()
    