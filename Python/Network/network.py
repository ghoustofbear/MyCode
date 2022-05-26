


from email import header

import re

import requests
class test:
    def DouBan250():
        urls='https://movie.douban.com/top250'
        params={
            'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.84 Safari/537.36'  
            }
        responses=requests.get(url=urls,headers=params)
        # print(responses.text)
        #解析数据
        obj=re.compile(r'<li>.*?<span class="title">(?P<name>.*?)</span>.*?<span class="rating_num".*?>(?P<score>.*?)</span>.*?</li>',re.S)
        #开始匹配
        result=obj.finditer(responses.text)
        for c in result:
            print(c.group('name'),c.group('score'))
        
        
# def func():
#     urs='https://movie.douban.com/top250'
#     header={
#         'User-Agent':'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/99.0.4844.84 Safari/537.36' }
#     resonse=requests.get(url=urs,headers=header)
#     #解析数据
#     obj=re.compile(r'<li>.*?<span class="title">(?P<name>.*?)</span>.*?<span class="rating_num".*?>(?P<score>.*?)</span>.*?</li>',re.S)
#     #开始匹配
#     # result=obj.findall(resonse.text)
#     # for it in result:
#     #     print(it)
#     #     print(it[0],it[1])
#     #     print('*'*50)
      
#     # s=resonse.json()
   
#     # print('s='+s)
def main():
    test.DouBan250()
if __name__=='__main__':
    main()