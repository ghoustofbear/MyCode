import string


def changeHex(s:string):
    s1=bytes.fromhex(s)
    High_4=(s1&0xff00)>>8
    Low_4=s1&0xff
    s2=Low_4+High_4
    print("s2=",s2)
    return s2
if __name__=="__main__":
    m="0x9c"
    changeHex(m)