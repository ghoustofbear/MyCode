class Solution1:
    def fib(self,n:int) ->int:
        if n<2:
            return n
        p=0
        q=0
        r=1
        for i in range(2,n+1):
            p=q
            q=r
            r=p+q
        return r
def main():
    c=Solution1()
    print("c="+str(c.fib(4)))
if __name__=="__main__":
    main()
