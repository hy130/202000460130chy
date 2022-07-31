project:birthday attack of reduced SM3

birthday attack的思想是提前存储一定数量的hash结果，将其存入到一个hash表中，之后再利用另外一个数通过遍历
来寻找碰撞，在实验中最高实现了48bit的碰撞

uint8_t input1[64] = {
0x01, 0x23, 0x45, 0x0A, 0x9E, 0xEB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
};
uint8_t input2[64] = {
0x14, 0x0E, 0x1B, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10,
0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
};

hash:0x99, 0x0C, 0x2E, 0x5F, 0x3E, 0x99
运行指导：直接运行即可，但需要事先导入openssl库，在所给代码中是进行56bit的碰撞，可能无法跑出结果，可对于
其比较减少一组
代码见birthday-attack.cpp
结果见图片birthday-attack


project:the Rho methon of reduced SM3

rho attack的思想是对于两个输入，一个输入每次进行一次hash，而另一个输入每次进行两次hash,对二者的结果前几bit进行比较
若相等则碰撞成功，最高实现了24bit碰撞

0x9D,0xC7,0xB6,0x81,0x26,0x16,0x55,0x4E,0x39,0x79,0xFF,0x9D,0x56,0xFD,0xCC,0xCC,
0x41,0xE8,0x22,0x2E,0xC4,0x35,0xCE,0x2B,0xB2,0xF8,0xDF,0xA0,0x86,0x31,0x56,0x0D

0xD9,0x6E,0x8A,0xB4,0x45,0xD6,0xB3,0x60,0xC6,0x37,0xD5,0x82,0x32,0x3D,0x92,0x75
0xA7,0x94,0x52,0xD4,0x6D,0xAD,0xA7,0x8D,0x0F,0xAC,0x9B,0x8D,0x54,0xD3,0xB9,0xFB

hash:0x95,0x32,0x40
运行指导：直接运行即可，需事先导入openssl库
代码见rho-attack.cpp
结果见图片：rho-attack


project:extension attack for SM3

extension attack的思想是，对于一组数据先进行hash，再对于另一组数据再进行hash，所得到的结果与
对两组数据合并在一起的数据直接hash效果相同，因在实验中直接调用了openssl库中的SM3函数，因此直
接使用了其中的update函数，注意两个encrypt函数并不相同

运行指导：直接运行即可，需事先导入openssl库
代码见extension-attack.cpp
结果见图片：extension-attack

注：具体的代码中都有关键位置的注释