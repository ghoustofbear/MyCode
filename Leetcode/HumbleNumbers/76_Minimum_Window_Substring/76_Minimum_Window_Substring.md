#### [76. 最小覆盖子串](https://leetcode-cn.com/problems/minimum-window-substring/)

难度困难1772

给你一个字符串 `s` 、一个字符串 `t` 。返回 `s` 中涵盖 `t` 所有字符的最小子串。如果 `s` 中不存在涵盖 `t` 所有字符的子串，则返回空字符串 `""` 。

 

**注意：**

- 对于 `t` 中重复字符，我们寻找的子字符串中该字符数量必须不少于 `t` 中该字符数量。
- 如果 `s` 中存在这样的子串，我们保证它是唯一的答案。

 

**示例 1：**

```
输入：s = "ADOBECODEBANC", t = "ABC"
输出："BANC"
```

**示例 2：**

```
输入：s = "a", t = "a"
输出："a"
```

**示例 3:**

```
输入: s = "a", t = "aa"
输出: ""
解释: t 中两个字符 'a' 均应包含在 s 的子串中，
因此没有符合条件的子字符串，返回空字符串。
```

 

**提示：**

- `1 <= s.length, t.length <= 105`
- `s` 和 `t` 由英文字母组成

 

**进阶：**你能设计一个在 `o(n)` 时间内解决此问题的算法吗？

通过次数262,719

提交次数598,867

#### 解放及其思路：

###### 双哈希表法：

```C++
class Solution {
public:
    std::string minWindow(std::string s, std::string t) {
        std::string result;
        std::unordered_map<char,int>words,windows;
        int left=0;
        
        for(auto& word:t)
        {
            words[word]+=1;
        }
        int count=0;
        for( int i=0;i<s.size();i++)
        { 
            
            windows[s[i]]++;
            if(windows[s[i]]==words[s[i]])
            {
                count+=1;
            }
            while(windows[s[left]]>words[s[left]])
            {
                
                windows[s[left++]]--;
            }
            if(count==t.size())
            {
                auto flag=result.empty()||i-left+1<result.size();
                if(result.empty()||i-left+1<result.size())
                {
                    result=s.substr(left,i-left+1);
                    
                }
            }
        }
        return result;
    }



};
```



##### 滑动数组+哈希表

```python
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
        
```

