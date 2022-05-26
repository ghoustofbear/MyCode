#### [509. 斐波那契数](https://leetcode-cn.com/problems/fibonacci-number/)

难度简单421收藏分享切换为英文接收动态反馈

**斐波那契数** （通常用 `F(n)` 表示）形成的序列称为 **斐波那契数列** 。该数列由 `0` 和 `1` 开始，后面的每一项数字都是前面两项数字的和。也就是：

```
F(0) = 0，F(1) = 1
F(n) = F(n - 1) + F(n - 2)，其中 n > 1
```

给定 `n` ，请计算 `F(n)` 。

 

**示例 1：**

```
输入：n = 2
输出：1
解释：F(2) = F(1) + F(0) = 1 + 0 = 1
```

**示例 2：**

```
输入：n = 3
输出：2
解释：F(3) = F(2) + F(1) = 1 + 1 = 2
```

**示例 3：**

```
输入：n = 4
输出：3
解释：F(4) = F(3) + F(2) = 2 + 1 = 3
```

 

**提示：**

- `0 <= n <= 30`

通过次数344,801

提交次数516,497

```py
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
```

