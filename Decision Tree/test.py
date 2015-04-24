#!/usr/bin/env python
# -*- coding: utf-8 -*-
#coding=utf-8
import matplotlib.pyplot as plt
axprops = dict(xticks=[], yticks=[])
ax = plt.subplot(111, frameon=False, **axprops)
plt.xlim(-300,0)
plt.ylim(-400, 0)
#plt.annotate('root', xy=(0,0),bbox=dict(boxstyle="circle",fc="0.4"))
f = open("out.txt")             # 返回一个文件对象
line = f.readline()             # 调用文件的 readline()方法
while line:
	linshi = line.split()
	a=float(linshi[3])
	b=float(linshi[1])
	c=float((a+b)/2) 
	d=float(float(float(linshi[4])+float(linshi[2]))/2)
	plt.text(c,d,linshi[0],va="center", ha="center",rotation=30)
	plt.annotate('',xytext=(a,linshi[4]),xy=(b,linshi[2]),va="center", ha="center",arrowprops=dict(arrowstyle="->", color='g'))
	plt.annotate(linshi[5], xy=(linshi[1],linshi[2]),bbox=dict(boxstyle="circle",fc="0.4"))
	line = f.readline()
f.close()
plt.show()