import hashlib#利用hashlib模块中的sm3函数
import math

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

p=0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3
a=0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498
b=0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A
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
            if p1%8==1 or p1%8==7:
                s=1
            if p1%8==3 or p1%8==5:
                s=-1
        if p1%8==3 and a1%4==3:
            s=-s
        if a1==1:
            return s
        else:
            return s*leg((p1%a1),a1)
    
def addset(m):#集合加法
    temp=hashlib.new("sm3",m)
    h=temp.hexdigest()
    while leg(int(h,16),p)!=1:#如果勒让德符号不为1，则不是二次剩余
        h=hashlib.new("sm3",m)#再次进行hash，直至可以映射到椭圆曲线
        h=h.hexdigest()
    if list1==[]:#如果集合为空，直接进行添加
        x1=int(h,16)
        y1=math.sqrt(x1*x1*x1+a*x1+b)
        list1.append((x1,y1))
    else:#如果集合不为空，将其与结果进行加法
        x1=int(h,16)
        y1=math.sqrt(x1*x1*x1+a*x1+b)
        list1.append((x1,y1))
        x,y=addition(x1,x,y1,y)

def divideset(m):#集合减法
    h=hashlib.new("sm3",m)
    h=temp.hexdigest()
    x1=int(h,16)
    y1=math.sqrt(x1*x1*x1+a*x1+b)
    list1.remove(x1,y1)#从列表中删除
    y1=p-y1
    x,y=addition(x,x1,y,y1)

    
