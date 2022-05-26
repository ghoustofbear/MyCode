#### [70. 爬楼梯](https://leetcode-cn.com/problems/climbing-stairs/)

难度简单2268收藏分享切换为英文接收动态反馈

假设你正在爬楼梯。需要 `n` 阶你才能到达楼顶。

每次你可以爬 `1` 或 `2` 个台阶。你有多少种不同的方法可以爬到楼顶呢？

 

**示例 1：**

```
输入：n = 2
输出：2
解释：有两种方法可以爬到楼顶。
1. 1 阶 + 1 阶
2. 2 阶
```

**示例 2：**

```
输入：n = 3
输出：3
解释：有三种方法可以爬到楼顶。
1. 1 阶 + 1 阶 + 1 阶
2. 1 阶 + 2 阶
3. 2 阶 + 1 阶
```

 

**提示：**

- `1 <= n <= 45`

通过次数746,960

提交次数1,395,588

```py
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
    
```

