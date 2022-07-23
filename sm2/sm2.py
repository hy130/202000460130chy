import hashlib#利用hashlib模块中的sm3函数
import math
import random
p=0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
a=0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
b=0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
xg=0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D
yg=0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2
n=0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7#阶
def inv(a,m):#求a关于m的逆函数
    x1,x2,x3=1,0,a
    y1,y2,y3=0,1,m
    while y3!=0:
        q=x3//y3
        t1,t2,t3=x1-q*y1,x2-q*y2,x3-q*y3
        x1,x2,x3=y1,y2,y3
        y1,y2,y3=t1,t2,t3
    return x1%m
    
def addition(x1,x2,y1,y2):#(x1,y1)+(x2,y2)
    if x2 == x1 and y2 == p-y1:
        return -1,-1#代表无穷远
    temp=0
    if x1!=x2:#根据算法描述进行加法运算
        inv_x=inv(x2-x1,p)
        temp=(y2-y1)*inv_x
    else:
        inv_y=inv(2*y1,p)
        temp=((3*(x1**2))+a)*inv_y
    
    x3=temp**temp-x1-x2
    y3=temp*(x1-x3)-y1
    return x3,y3

def multi(x1,y1,k):#k(x1,y1)
    k=bin(k)[2:]#将k转化成二进制
    x2,y2=x1,y1
    for i in range(1,len(k)):
        x2,y2=addition(x2,x2,y2,y2)
        if k[i]=='1':
            x2,y2=addition(x2,x1,y2,y1)
    return x2,y2

def KDF(z,Klen):#密钥扩展算法
    ct=1
    k=''
    for i in range(math.ceil(Klen/256)):#向上取整
        h=hashlib.new("sm3",(hex(int(z+'{:032b}'.format(ct),2))[2:]).encode())#填充到32bit
        k=k+h.hexdigest()
        ct=ct+1
    k='0'*(((256-(len(bin(int(k,16)))-2)%256))%256)+bin(int(k,16))[2:]#填充到256bit倍数
    return k[:Klen]#返回要获得的密钥长度

xb=0x435B39CCA8F3B508C1488AFC67BE491A0F7BA07E581A0E4849A5CF70628A7E0A#用户B的公钥
yb=0x75DDBA78F15FEECB4C7895E2C1CDF5FE01DEBB2CDBADF45399CCF77BBA076A42
dB=0x1649AB77A00637BD5E2EFE283FBF353534AA7F7CB89463F208DDBC2920BB0DA0
m=0x656E6372797074696F6E207374616E64617264

def encrypt(m):
    klen=len(bin(m)[2:])#获取m二进制比特串长度
    x1,x2,y1,y2=0,0,0,0
    t=''
    while True:
        #k=1
        k=random.randint(1,n)#生成随机数k
        #k=0x4C62EEFD6ECFC2B95B92FD6C3D9575148AFA17425546D49018E5388D49DD7B4F
        x1,y1=multi(xg,yg,k)#基点倍数
        print(x1,y1)
        x2,y2=multi(xb,yb,k)#用户B公钥倍数
        t=t+KDF('{:0256b}'.format(x2)+'{:0256b}'.format(y2),klen)
        if int(t,2)!=0:
            break
    plen=len(hex(p)[2:])#p的十六进制长度
    c1='04'+(plen-len(hex(x1)[2:]))*'0'+hex(x1)[2:]+(plen-len(hex(y1)[2:]))*'0'+hex(y1)[2:]#填充到与plen相同长度，便于分割
    c2=hex(m^int(t,2))[2:]
    h=hashlib.new("sm3",(hex(x2)[2:]+hex(m)[2:]+hex(y2)[2:]).encode())
    c3=h.hexdigest()
    return c1,c2,c3

def decrypt(c1,c2,c3,dB):
    c1=c1[2:]#去除填充
    x1,y1=int(c1[:len(c1)//2],16),int(c1[len(c1)//2:],16)
    print(x1,y1)
    if pow(y1,2,p)!=(pow(x1,3,p)+a*x1+b)%p: #(x1,y1)不在椭圆上报错
        return False    
    x2,y2=multi(x1,y1,dB)
    klen=len(bin(c2))
    t=KDF(hex(x2)[2:]+hex(y2)[2:],klen)#计算KDF
    if int(t,2)==0:
        return False
    m='0'*(klen-len(bin(int(c2,16)^int(t,2))[2:]))+bin(int(c2,16)^int(t,2))[2:]#填充
    h=hashlib.new("sm3",hex(int(x2+m+y2,2))[2:])
    if h.hexdigest!=c3:
        return False
    return hex(int(m,2))[2:]
    
c1,c2,c3=encrypt(m)
decrypt(c1,c2,c3,dB)


        
        
