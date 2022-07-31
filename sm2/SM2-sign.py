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

M="thisistheplaintext"#所发送的信息
Z="iambob"
if __name__=="__main__":
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)#建立一个套接字
    host='127.0.0.1'
    port=8090
    
    s.bind((host,port))#绑定该端口
    s.listen(1)#进行监听

    c,addr=s.accept()
    print('address:',addr)

    d1=random.randint(1,n)#签名一系列操作
    xp1,yp1=multi(xg,yg,gmpy2.invert(d1,p))

    c.send(str(xp1).encode())#发送信息，有时间间隔
    time.sleep(1)
    c.send(str(yp1).encode())
    time.sleep(1)

    h=hashlib.new("sm3",(Z+M).encode())
    e=h.hexdigest()
    k1=random.randint(1,n)
    xq1,yq1=multi(xg,yg,k1)

    c.send(str(xq1).encode())#继续发送信息
    time.sleep(1)
    c.send(str(yq1).encode())
    time.sleep(1)

    c.send(e.encode())

    r=c.recv(1024)#接收到的信息
    s2=c.recv(1024)
    s3=c.recv(1024)

    r=int(r.decode(),10)#进行计算产生签名
    s2=int(s2.decode(),10)
    s3=int(s3.decode(),10)

    s_=((d1*k1)*s2+d1*s3-r)%n
    print(hex(r)[2:],hex(s_)[2:])





    

    
