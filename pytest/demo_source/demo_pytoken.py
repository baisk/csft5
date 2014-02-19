#encoding=utf8
import jieba
jieba.initialize() #阻止延迟绑定功能
#第一版的python 分词接口 供cython调用, 测试使用

'''
两个必须实现的函数:
SetDocument()
GetToken()

GetAdvToken() #返回每个词的 分词, 同义词, 词性标注
'''


class Test_token(object):
	"""docstring for Test_token"""
	def __init__(self):
		self.word = None

	def GetToken(self):
		print "########in demo_pytoken: GetToken"
		#return [w.encode('utf8') for w in self.token]  #should be utf8 coding
		return self.token

	def GetAdvToken(self):
		#return [ ( w.encode('utf8'), None, None) for w in self.token]  #返回每个词的 分词, 同义词, 词性标注
		return [(w, None, None) for w in self.GetToken()]  ##最简单的返回格式

	def SetDocument(self, word):
		self.word = word
		self.token = list(jieba.cut(self.word))
		return True

if __name__ == '__main__':
	tk = Test_token()
	tk.SetDocument("中国你好")
	for i in tk.GetToken():
		print i
