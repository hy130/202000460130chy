import hashlib
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
d=0#私钥
xq=0#公钥
yq=0
def keygen():#生成公私钥
    d=random.randint(n)
    xq,yq=multi(xg,yg,d)
    
def sig(m):
    k,x1,y1,r=0,0,0,0
    while True:
        k=random.randint(n)
        x1,y1=multi(xg,yg,k)
        r=x1%n
        if r!=0:
            break
    t=inv(k,n)
    h=hashlib.new("sha1",m)
    h=h.hexdigest()
    e=int(h,16)
    s=(t*(h+d*r))%n
    return r,s

def ver(r,s,m):
    h=hashlib.new("sha1",m)
    h=h.hexdigest()
    e=int(h,16)
    w=inv(s,n)
    u=(e*w)%n
    v=(r*w)%n
    x1,y1=addition(multi(xg,yg,u),multi(xg,yg,v))
    if(x1==-1):
        return False
    if x1==r:
        return True
    return False

def forge(r,s):
    u=random.randint(1,3)
    v=random.randint(1,3)
    #print(u,v)
    x1,y1=multi(xg,yg,u)
    x2,y2=multi(xq,yq,v)
    x_f,y_f=addition(x1,x2,y1,y2)
    r_f=x_f%n
    e_f=(r_f*u*inv(v,n))%n
    s_f=(r_f*inv(v,n))%n
    return r_f,s_f

#获取的签名数据
r1="00d71db39ae4f6bc3128d78661f6951bb17b097ab32e1ead3ce6fe761914715dd5"
s1="1970b34d36a9fef053b8b7a3c4555ef5bba14a88992449b980aee862465d966e"
xq="04ac8c30c4e85f47702bfa8d8d56d8e6"
yq="14258958d6655611474da6cadb8c7319"
r1=int(r1,16)
s1=int(s1,16)
r_,s_=forge(r1,s1)

    
