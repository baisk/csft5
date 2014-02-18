#encoding=utf8
import jieba
#第一版的python 分词接口 供cython调用, 测试使用

'''
两个必须实现的函数:
SetBuffer()
GetToken()
'''


class Test_token(object):
	"""docstring for Test_token"""
	def __init__(self):
		self.word = None

	def GetToken(self):
		words =  list(jieba.cut(self.word))
		return [w.encode('utf8') for w in words]  #should be utf8 coding

	def SetBuffer(self, word):
		print "########in demo_pytoken: SetBuffer"
		if type(word) == str:
			word = word.decode('utf8')
		self.word = word


	def Echo(self):
		print '########in demo_pytoken: Echo'
		return 'echo in token class'

if __name__ == '__main__':
	tk = Test_token()
	tk.SetBuffer("中国你好")
	for i in tk.GetToken():
		print i
