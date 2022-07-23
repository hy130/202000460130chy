//Merkle Patricia Trie�����ֽڵ㣺
////�սڵ㣺�ýڵ����������б�ʾ�մ�����ʵ������
//Ҷ�ӽڵ㣺����ѧ��Ҷ�ӽڵ������ͬ��Ϊһ��keyֵ��valueֵ����ɵ��б�
//��չ�ڵ㣺ҲΪkey��value��ɵ��б������е�value�������ڵ��hash�������ô����ӵ������ڵ�
//��֧�ڵ㣺ӵ��һ�����ӽڵ����ϵķ�Ҷ�ӽڵ㣬�䳤��Ϊ17
type node interface {
	fstring(string) string
	cache() (hashNode, bool)
	canUnload(cachegen, cachelimit uint16) bool
}

type(
	fullNode struct {//��֧�ڵ�Ķ���
		Children[17]node //����17�����ӽڵ�
		flags    nodeFlag
	}
	shortNode struct {//��Ӧ����չ�ڵ��Ҷ�ӽڵ�Ķ���
		Key[]byte//keyֵ
		Val   node//valueֵ
		flags nodeFlag
	}
	hashNode[]byte
	valueNode[]byte
)
type Trie struct {//���Ľṹ��
	root         node//���ڵ�
	db           Database//��˵Ĵ洢�����ݿ�洢ʱ�õ�
	originalRoot common.Hash//


	cachegen, cachelimit uint16
}
//Trie���ĳ�ʼ������
//�ú�����������������һ����Hashһ�������ݿ�Database
//��Hash��Ϊ������һ�����ݿ��е��Ѵ�����
func New(root common.Hash, db Database) (*Trie, error) {
	trie: = &Trie{ db: db, originalRoot : root }
	if (root != common.Hash{}) && root != emptyRoot{//����Ϊ��
		if db == nil {
			panic("trie.New: cannot use existing root without a database")
		}
		rootnode, err : = trie.resolveHash(root[:], nil)//�ú����������ظ���
		if err != nil {
			return nil, err
		}
		trie.root = rootnode
	}
	return trie, nil//���򷵻��½���
}
//Trie���Ĳ���
//�Ӹ��ڵ㿪ʼ�ݹ�����Ѱ��
//prefix���Ѵ����key��key��δ�����key���Դ�����ݹ�������
//node������ɵ��������ڵ�
func(t* Trie) insert(n node, prefix, key[]byte, value node) (bool, node, error) {
	if len(key) == 0 {//keyֵ����Ϊ0����ƥ����ɻ�������
		if v, ok : = n.(valueNode); ok{
			return !bytes.Equal(v, value.(valueNode)), value, nil
		}
			return true, value, nil
	}
	switch n : = n.(type) {
	case* shortNode://Ҷ�ӽڵ����
		matchlen: = prefixLen(key, n.Key)//����ǰ׺����
		if matchlen == len(n.Key) {//keyֵһ�������
			dirty, nn, err : = t.insert(n.Val, append(prefix, key[:matchlen]...), key[matchlen:], value)
				if !dirty || err != nil{//valueֵ��Ҳһ����ʾ�Ѵ��ڣ�����
					return false, n, err
				}
				return true, & shortNode{ n.Key, nn, t.newFlag() }, nil//û������������
		}
		branch: = &fullNode{ flags: t.newFlag() }//����ȫƥ������
		var err error
		//��ȡ����ǰ׺����һ����չ�ڵ㣬����һ��branch�ڵ�
		_, branch.Children[n.Key[matchlen]], err = t.insert(nil, append(prefix, n.Key[:matchlen + 1]...), n.Key[matchlen + 1:], n.Val)
		if err != nil{
			return false, nil, err
		}
		_, branch.Children[key[matchlen]], err = t.insert(nil, append(prefix, key[:matchlen + 1]...), key[matchlen + 1:], value)
		if err != nil{
			return false, nil, err
		}
		//������ǰ׺Ϊ0��branch����Ҷ�ӽڵ�
		if matchlen == 0 {
			return true, branch, nil
		}
		//����Ļ���Ҷ�ӽڵ����
		return true, & shortNode{ key[:matchlen], branch, t.newFlag() }, nil
	
	case* fullNode://fullNode�ڵ����
		//ֱ�ӵ���insert���������ӽڵ����
		dirty, nn, err : = t.insert(n.Children[key[0]], append(prefix, key[0]), key[1:], value)
		if !dirty || err != nil{
			return false, n, err//����
		}
		n = n.copy()
		n.flags = t.newFlag()
		n.Children[key[0]] = nn
		return true, n, nil

	case nil://ȫ�µ������½�һ��shortNode�ڵ�
		return true, & shortNode{ key, value, t.newFlag() }, nil

	case hashNode://��ǰ�ڵ���hashNode
		//˵����ǰ�ڵ�δ���ص��ڴ�
		rn, err : = t.resolveHash(n, prefix)//���ýڵ���ص��ڴ�
		if err != nil{
			return false, nil, err
		}
		dirty, nn, err : = t.insert(rn, prefix, key, value)//��������Ӧλ��
		if !dirty || err != nil{
			return false, rn, err
		}
		return true, nn, nil

	default:
		panic(fmt.Sprintf("%T: invalid node: %v", n, n))//����
	}
}
//Trie��������������Trie������ȡ��Ӧ��key����Ϣ
func(t* Trie) tryGet(origNode node, key[]byte, pos int) (value[]byte, newnode node, didResolve bool, err error) {
	switch n : = (origNode).(type) {
	case nil://ȫ�µ������
		return nil, nil, false, nil
	case valueNode:
		return n, n, false, nil
	case* shortNode://Ҷ�ӽڵ����
		if len(key) - pos < len(n.Key) || !bytes.Equal(n.Key, key[pos:pos + len(n.Key)]) {
			//û�������з��ֶ�Ӧkeyֵ
			return nil, n, false, nil//����
		}
		value, newnode, didResolve, err = t.tryGet(n.Val, key, pos + len(n.Key))//�����ݹ�
		if err == nil && didResolve{//�ҵ��˶�Ӧֵ�����临�Ʒ���
			n = n.copy()
			n.Val = newnode
			n.flags.gen = t.cachegen
		}
		return value, n, didResolve, err
	case* fullNode://fullNode���
		value, newnode, didResolve, err = t.tryGet(n.Children[key[pos]], key, pos + 1)//�����ݹ�
		if err == nil && didResolve{//�ҵ��˶�Ӧֵ�����临�Ʒ���
			n = n.copy()
			n.flags.gen = t.cachegen
			n.Children[key[pos]] = newnode
		}
		return value, n, didResolve, err/
	case hashNode://hashNode���
		child, err : = t.resolveHash(n, key[:pos])//������ص��ڴ�
		if err != nil{
			return nil, n, true, err
		}
		value, newnode, _, err : = t.tryGet(child, key, pos)
		return value, newnode, true, err
	default:
		panic(fmt.Sprintf("%T: invalid node: %v", origNode, origNode))//����
	}
}