//Merkle Patricia Trie有四种节点：
////空节点：该节点用来在树中表示空串，无实际意义
//叶子节点：与所学的叶子节点大致相同，为一个key值和value值所组成的列表
//扩展节点：也为key、value组成的列表，但其中的value是其他节点的hash，可以用此连接到其他节点
//分支节点：拥有一个孩子节点以上的非叶子节点，其长度为17
type node interface {
	fstring(string) string
	cache() (hashNode, bool)
	canUnload(cachegen, cachelimit uint16) bool
}

type(
	fullNode struct {//分支节点的定义
		Children[17]node //具有17个孩子节点
		flags    nodeFlag
	}
	shortNode struct {//对应于扩展节点和叶子节点的定义
		Key[]byte//key值
		Val   node//value值
		flags nodeFlag
	}
	hashNode[]byte
	valueNode[]byte
)
type Trie struct {//树的结构体
	root         node//根节点
	db           Database//后端的存储，数据库存储时用到
	originalRoot common.Hash//


	cachegen, cachelimit uint16
}
//Trie树的初始化函数
//该函数具有两个参数，一个是Hash一个是数据库Database
//若Hash不为空则是一个数据库中的已存在树
func New(root common.Hash, db Database) (*Trie, error) {
	trie: = &Trie{ db: db, originalRoot : root }
	if (root != common.Hash{}) && root != emptyRoot{//若不为空
		if db == nil {
			panic("trie.New: cannot use existing root without a database")
		}
		rootnode, err : = trie.resolveHash(root[:], nil)//该函数用来加载该树
		if err != nil {
			return nil, err
		}
		trie.root = rootnode
	}
	return trie, nil//否则返回新建树
}
//Trie树的插入
//从根节点开始递归向下寻找
//prefix是已处理的key，key是未处理的key（以此满足递归条件）
//node插入完成的子树根节点
func(t* Trie) insert(n node, prefix, key[]byte, value node) (bool, node, error) {
	if len(key) == 0 {//key值长度为0，以匹配完成或发生错误
		if v, ok : = n.(valueNode); ok{
			return !bytes.Equal(v, value.(valueNode)), value, nil
		}
			return true, value, nil
	}
	switch n : = n.(type) {
	case* shortNode://叶子节点情况
		matchlen: = prefixLen(key, n.Key)//公共前缀长度
		if matchlen == len(n.Key) {//key值一样的情况
			dirty, nn, err : = t.insert(n.Val, append(prefix, key[:matchlen]...), key[matchlen:], value)
				if !dirty || err != nil{//value值若也一样表示已存在，报错
					return false, n, err
				}
				return true, & shortNode{ n.Key, nn, t.newFlag() }, nil//没错误正常更新
		}
		branch: = &fullNode{ flags: t.newFlag() }//不完全匹配的情况
		var err error
		//提取公共前缀构成一个扩展节点，其后跟一个branch节点
		_, branch.Children[n.Key[matchlen]], err = t.insert(nil, append(prefix, n.Key[:matchlen + 1]...), n.Key[matchlen + 1:], n.Val)
		if err != nil{
			return false, nil, err
		}
		_, branch.Children[key[matchlen]], err = t.insert(nil, append(prefix, key[:matchlen + 1]...), key[matchlen + 1:], value)
		if err != nil{
			return false, nil, err
		}
		//若公共前缀为0用branch代替叶子节点
		if matchlen == 0 {
			return true, branch, nil
		}
		//否则的话用叶子节点代替
		return true, & shortNode{ key[:matchlen], branch, t.newFlag() }, nil
	
	case* fullNode://fullNode节点情况
		//直接调用insert函数往孩子节点添加
		dirty, nn, err : = t.insert(n.Children[key[0]], append(prefix, key[0]), key[1:], value)
		if !dirty || err != nil{
			return false, n, err//报错
		}
		n = n.copy()
		n.flags = t.newFlag()
		n.Children[key[0]] = nn
		return true, n, nil

	case nil://全新的树，新建一个shortNode节点
		return true, & shortNode{ key, value, t.newFlag() }, nil

	case hashNode://当前节点是hashNode
		//说明当前节点未加载到内存
		rn, err : = t.resolveHash(n, prefix)//将该节点加载到内存
		if err != nil{
			return false, nil, err
		}
		dirty, nn, err : = t.insert(rn, prefix, key, value)//将其插入对应位置
		if !dirty || err != nil{
			return false, rn, err
		}
		return true, nn, nil

	default:
		panic(fmt.Sprintf("%T: invalid node: %v", n, n))//报错
	}
}
//Trie函数，遍历整个Trie树，获取对应的key的信息
func(t* Trie) tryGet(origNode node, key[]byte, pos int) (value[]byte, newnode node, didResolve bool, err error) {
	switch n : = (origNode).(type) {
	case nil://全新的树情况
		return nil, nil, false, nil
	case valueNode:
		return n, n, false, nil
	case* shortNode://叶子节点情况
		if len(key) - pos < len(n.Key) || !bytes.Equal(n.Key, key[pos:pos + len(n.Key)]) {
			//没有在树中发现对应key值
			return nil, n, false, nil//报错
		}
		value, newnode, didResolve, err = t.tryGet(n.Val, key, pos + len(n.Key))//继续递归
		if err == nil && didResolve{//找到了对应值，将其复制返回
			n = n.copy()
			n.Val = newnode
			n.flags.gen = t.cachegen
		}
		return value, n, didResolve, err
	case* fullNode://fullNode情况
		value, newnode, didResolve, err = t.tryGet(n.Children[key[pos]], key, pos + 1)//继续递归
		if err == nil && didResolve{//找到了对应值，将其复制返回
			n = n.copy()
			n.flags.gen = t.cachegen
			n.Children[key[pos]] = newnode
		}
		return value, n, didResolve, err/
	case hashNode://hashNode情况
		child, err : = t.resolveHash(n, key[:pos])//将其加载到内存
		if err != nil{
			return nil, n, true, err
		}
		value, newnode, _, err : = t.tryGet(child, key, pos)
		return value, newnode, true, err
	default:
		panic(fmt.Sprintf("%T: invalid node: %v", origNode, origNode))//报错
	}
}