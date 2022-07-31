import hashlib#利用hashlib模块中的sm3函数
import math
import gmpy2

p=gmpy2.mpz(0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3)
a=gmpy2.mpz(0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498)
b=gmpy2.mpz(0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A)
xg=gmpy2.mpz(0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D)
yg=gmpy2.mpz(0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2)
n=gmpy2.mpz(0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7)#阶
'''
p=11
a=1
b=6
xg=2
yg=7
n=13
'''
def sqrt_p(a):
    for i in range(p):
        if (i*i)%p==a:
            return i
def addition(x1,x2,y1,y2):#(x1,y1)+(x2,y2)
    if x2 == x1 and y2 == p-y1:
        return -1,-1#代表无穷远
    temp=0
    if x1!=x2:#根据算法描述进行加法运算
        temp=((y2-y1)*gmpy2.invert(x2-x1,p))%p
    else:
        temp=(((3*(x1**2))+a)*gmpy2.invert(2*y1,p))%p
    
    x3=(temp*temp-x1-x2)%p
    y3=(temp*(x1-x3)-y1)%p
    return x3,y3

list1=[]#用一个列表存取加入的数据
x=0#现在的集合结果
y=0

def leg(a1,p):#勒让德符号用来判断是否为二次剩余
    if a1==0:
        return 0
    elif a1==1:
        return 1
    else:
        e=0
        while(a1%2==0):
            a1=a1/2
            e=e+1
        if e%2==0:
            s=1
        else:
            if p%8==1 or p%8==7:
                s=1
            if p%8==3 or p%8==5:
                s=-1
        if p%8==3 and a1%4==3:
            s=-s
        if a1==1:
            return s
        else:
            return s*leg((p%a1),a1)
    
def addset(m):#集合加法
    temp=hashlib.new("sm3",m.encode())
    h=temp.hexdigest()
    x1=int(h,16)
    y1=0
    k=(x1*x1*x1+a*x1+b)%p
    print(k)
    while True:
        if leg(k,p)!=1:
            h=hashlib.new("sm3",hex(k).encode())#再次进行hash，直至可以映射到椭圆曲线
            k=int(h.hexdigest(),16)
            print(k)
        else:
            y1=sqrt_p(k)%p
            break
        
    if list1==[]:#如果集合为空，直接进行添加
        list1.append((x1,y1))
        x=x1
        y=y1
    else:#如果集合不为空，将其与结果进行加法
        list1.append((x1,y1))
        x,y=addition(x1,x,y1,y)

def divideset(m):#集合减法
    h=hashlib.new("sm3",m.encode())
    h=h.hexdigest()
    x1=int(h,16)
    y1=sqrt_p(x1*x1*x1+a*x1+b)
    list1.remove((x1,y1))#从列表中删除
    y1=p-y1
    x1,y1=addition(x,x1,y,y1)

addset("123")
