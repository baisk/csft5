import importlib
def test():
    module_name, cName = 'demo_pysource', 'TestSource'
    try:
        m = importlib.import_module(module_name)
        c = getattr(m, cName)
        return c
    except ImportError, e:
        print e
        return None

c = test()
print c


import os
print os.getcwd()