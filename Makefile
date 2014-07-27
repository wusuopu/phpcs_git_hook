all:
	python2 setup.py build

clean:
	rm -rf build

install:
	python2 setup.py install
	ln -vfs `pwd`/phpcs/scripts/phpcs /usr/bin/
	ln -vfs `pwd`/pre-commit /usr/share/git-core/templates/hooks/

uninstall:
	rm -v /usr/bin/phpcs
	pip2 uninstall lcgit

update:
	git pull
