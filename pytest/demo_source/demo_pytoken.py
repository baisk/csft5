#encoding=utf8
import jieba
jieba.initialize() #阻止延迟绑定功能
#第一版的python 分词接口 供cython调用, 测试使用

'''
两个必须实现的函数:
SetDocument()
GetToken()

额外的函数:
GetAdvToken() #返回每个词的 分词, 同义词, 词性标注
'''

class Test_token(object):
	"""docstring for Test_token"""
	def __init__(self):
		self.word = None

	def GetToken(self):
		#print "########in demo_pytoken: GetToken"
		#return [w.encode('utf8') for w in self.token]  #shodecreatePythonTokenizerObjectuld be utf8 coding
		return self._filter( [w for w in self.token] )

	def GetAdvToken(self):
		#return [ ( w.encode('utf8'), None, None) for w in self.token]  #返回每个词的 分词, 同义词, 词性标注
		return [(w, u'爆炸', None) for w in self.GetToken()]  ##最简单的返回格式 测试同义词扩展

	def SetDocument(self, word):
		self.word = word
		self.token = list(jieba.cut(self.word))
		return True

	def _filter(self, l_terms):  # 一个简单的过滤.
		#l_terms is a list of terms
		tmp_terms = map(lambda x: x.strip().lower(),  l_terms)  #全部换算成小写

		return [w for w in tmp_terms if w]



if __name__ == '__main__':
	tk = Test_token()
	tk.SetDocument("第一：private, public, protected 访问标号的访问范围。\n")
	for i in tk.GetToken():
		print i.encode('utf8')
