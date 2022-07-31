project:send a tx on Bitcoin testnet,parse the tx data down to every bit

通过利用比特币测试网新建一个钱包，再利用https://testnet.manu.backend.hamburg/faucet网址获取一定数量的比特币
，之后得到其中的数据，主要是对其中的hex数据即为RAW数据来进行分析，
具体内容见txt


project:forge a signature to pretend that you are Satoshi

在网上找到一个签名的脚本，对该脚本进行数据分析。数据脚本的位置数据见图ScriptSig，之后再按照课件所给的
相应的方法对此进行伪造。

运行指导：直接运行即可
代码见ecdsa_sig.py
运行结果见图forge-sign
