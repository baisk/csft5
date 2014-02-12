import jieba
#第一版的python 分词接口 供cython调用, 测试使用

class Test_token(object):
	"""docstring for Test_token"""
	def __init__(self, arg):
		super(Test_token, self).__init__()
		self.arg = arg

	def Token(self, word):
		if type(word) == str:
			word = word.decode('utf8')
		return list(jieba.cut(word))