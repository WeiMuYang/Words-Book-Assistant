import os
import sys
import codecs
import chardet

def convert(filename,out_enc="utf8"):
  try:
    content=codecs.open(filename,'rb+').read()
    source_encoding=chardet.detect(content)["encoding"]
    print(source_encoding)
    
    if source_encoding == "GB2312":
      content=content.decode(source_encoding).encode(out_enc)
      codecs.open(filename,'wb+').write(content)
  except IOError as err:
    print("I/O error:{0}".format(err))

def removeBom(file):
  '''移除UTF-8文件的BOM字节'''
  data = open(file,'rb+').read()
  if data[:3] == codecs.BOM_UTF8:
    data = data[3:]
    data.decode("utf-8")
    # print(data.decode("utf-8"))


def explore(dir):
  for root,dirs,files in os.walk(dir):
    for file in files:
      if os.path.splitext(file)[1]=='.cs':
        print(file)
        path=os.path.join(root,file)
        convert(path)
        removeBom(path)

def main():
  explore(sys.argv[1])

if __name__=="__main__":
  main() 
