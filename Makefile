default: build

FILES=py2cpp setup.py README.md MANIFEST.in

py2cpp:
	cython topwords/py2cpp.pyx -o topwords/py2cpp.by_cython.cpp --cplus

build: ${FILES}
	python3 setup.py build_ext -i

sdist: ${FILES}
	python3 setup.py sdist

publish: clean sdist
	twine upload dist/*

clean:
	rm -rf dist build topwords.egg-info
	rm -f topwords/*.by_cython.* *.so

