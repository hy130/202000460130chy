import hashlib#利用hashlib模块中的sm3函数
import math
import random
import gmpy2
import socket
import time
p=gmpy2.mpz(0x8542D69E4C044F18E8B92435BF6FF7DE457283915C45517D722EDB8B08F1DFC3)
a=gmpy2.mpz(0x787968B4FA32C3FD2417842E73BBFEFF2F3C848B6831D7E0EC65228B3937E498)
b=gmpy2.mpz(0x63E4C6D3B23B0C849CF84241484BFE48F61D59A5B16BA06E6E12D1DA27C5249A)
xg=gmpy2.mpz(0x421DEBD61B62EAB6746434EBC3CC315E32220B3BADD50BDC4C4E6C147FEDD43D)
yg=gmpy2.mpz(0x0680512BCBB42C07D47349D2153B70C4E5D7FDFCBFA36EA1A85841B9E46E09A2)
n=gmpy2.mpz(0x8542D69E4C044F18E8B92435BF6FF7DD297720630485628D5AE74EE7C32E79B7)#阶
    
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

def multi(x1,y1,k):#k(x1,y1)
    k=bin(k)[2:]#将k转化成二进制
    #print(k)
    x2,y2=x1,y1
    for i in range(1,len(k)):
        #print(i)
        x2,y2=addition(x2,x2,y2,y2)
        if k[i]=='1':
            x2,y2=addition(x2,x1,y2,y1)
    return x2,y2

if __name__=="__main__":
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)#建立一个套接字
    host='127.0.0.1'
    port=8090

    s.connect((host,port))#连接该端口
    
    xp1=s.recv(1024)#接收
    yp1=s.recv(1024)

    xp1=int(xp1.decode(),10)#进行解码转化为10进制
    yp1=int(yp1.decode(),10)

    d2=random.randint(1,n)#进行签名的一系列操作
    x_,y_=multi(xp1,yp1,gmpy2.invert(d2,p))
    P=addition(x_,xg,y_,p-yg)

    xq1=s.recv(1024)#接收信息
    yq1=s.recv(1024)
    e=s.recv(1024)

    xq1=int(xq1.decode(),10)#解码为10进制
    yq1=int(yq1.decode(),10)
    e=e.decode()
    e=int(e,16)
    
    k2=random.randint(1,n)#签名的一系列操作
    xq2,yq2=multi(xg,yg,k2)
    k3=random.randint(1,n)
    x_1,y_1=multi(xq1,yq1,k3)
    x1,y1=addition(x_1,xq2,y_1,yq1)
    r=(x1+e)%n
    s2=(d2*k3)%3
    s3=(d2*(r+k2))%n

    s.send(str(r).encode())#继续发送信息
    time.sleep(1)
    s.send(str(s2).encode())
    time.sleep(1)
    s.send(str(s3).encode())
    
