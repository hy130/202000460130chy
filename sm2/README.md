project: SM2

实验中首先尝试实现sm2，实验中因具体的数据格式未找到一个统一各格式
对于数据格式以及bit数参考了：https://blog.csdn.net/qq_33439662/article/details/122590298
之后便实现椭圆曲线，对于其使用了gmpy2模块中的一些函数，关于里面的KDF以及加密解密函数均
参照了相应的国家标准文件，根据文件中的步骤逐步进行
其中的hash函数sm3则是利用了hashlib模块中的sm3

运行指导：直接运行即可
代码见sm2.py
运行结果：见图片sm2


project:ECMH scheme

ECMH的实现，对于其中的一个人256bit的hash值看成一个大数，在椭圆曲线上寻找对应的值，
及对于结果进行勒让德符号判断是否为二次剩余，如果是则表明在该椭圆曲线上，对该值进行
加法存储，并将对应的点进行记录。如果不是二次剩余，实验中采用的方法是再次进行一次hash，因有一半二次剩余，
所以进行不了多少次

运行指导：直接运行即可
代码见sm2-sethash.py
运行结果：因未找到一个可以有效计算二次剩余的方法，未表现出结果


project:PGP scheme with SM2

PGP的实现，在实验中利用了socket套接字，利用该套接字实现了服务器与客户端之间的交互
其中的密钥生成等步骤皆按照其课件所给的步骤进行，并利用了SM2所实现的加密功能来对于此进行实现
每次发送一个数据后将其暂停1s来防止数据冲突

运行指导：先运行PGP，再运行PGP-client
代码见PGP.py和PGP-client.py
运行结果：见图片PGP-server和PGP-client


project:sm2 2p sign with real network communication

SM2-sign实现，在实验中也使用了socket套接字，具体的实现参照着相应的签名步骤，
socket的使用与PGP没有太大的差异

运行指导：先运行SM2-sign，再运行SM2-signclient
代码见SM2-sign.py和SM2-signclient.py
运行结果：见图片SM2-sign


project:sm2 2p decrypt with real network communication

该实验中也使用了套接字，具体的步骤根据课件上的过程，首先由一个第三方在得知d1,d2之后利用
其计算出公钥，利用该公钥进行加密得到c1,c2,c3，将其发送给d1生成方，d1生成方只能在d2
生成方协助下才能够进行解密
运行指导：先运行decrypt.py，再运行decrypt-client.py
代码见decrypt.py和decrypt-client.py
运行结果见图片decrypt

注：关键步骤代码中皆有注释
