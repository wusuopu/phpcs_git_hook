#!/usr/bin/env python
#-*- coding:utf-8 -*-

from setuptools import setup, Extension

lcgit = Extension('lcgit', sources=["git.c"], libraries=['git2'])
setup(
    name='lcgit',
    version='0.1',
    author='wusuopu',
    author_email='admin@longchangjin.cn',
    platforms=['Linux'],
    ext_modules=[lcgit]
)
