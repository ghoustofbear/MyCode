
from selenium import webdriver
import time
class zhihu:
    def test():
        global driver
        urls='https://www.zhihu.com'
        options=webdriver.ChromeOptions()
        options.add_experimental_option("excludeSwitches", ["enable-automation"])
        options.add_experimental_option('useAutomationExtension', False)
        driver = webdriver.Chrome(options=options, executable_path='C:\\Program Files\\Google\Chrome\\Application\\chromedriver.exe')
        driver.execute_cdp_cmd("Page.addScriptToEvaluateOnNewDocument", {
        "source": """
        Object.defineProperty(navigator, 'webdriver', {
        get: () => undefined
        })
        """
        })
        driver.get(urls)

        
        driver.find_element_by_xpath('//*[@id="root"]/div/main/div/div/div/div/div[1]/div/div[1]/form/div[1]/div[2]').click()
        time.sleep(1)
        driver.find_element_by_xpath('//*[@id="root"]/div/main/div/div/div/div/div[1]/div/div[1]/form/div[2]/div/label/input').send_keys('1056203382@qq.com') 
        driver.find_element_by_xpath('//*[@id="root"]/div/main/div/div/div/div/div[1]/div/div[1]/form/div[3]/div/label/input').send_keys('lx153294@ndy')
        driver.find_element_by_xpath('//*[@id="root"]/div/main/div/div/div/div/div[1]/div/div[1]/form/button').click()
        

def main():
    zhihu.test()
if __name__=='__main__':
    main()