# Cumbia *hdb* and *hdb++* historical databases documentation on github.io

## Regenerate doc from source (sources are not in gh-pages branch)

- cd cumbia-hdb/builddir
- rm doc/html/ -rf && ninja

## Copy doc from source tree to gh-pages branch

Suppose a copy of the *gh-pages* branch is in ../../cumbia-historicaldb-gh-pages/ respect to the *builddir* folder under cumbia-hdb

cp -a doc/html/* ../../cumbia-historicaldb-gh-pages/ 

## Commit and push new documentation to github gh-pages branch

- cd ../../cumbia-historicaldb-gh-pages

- git commit -a -m "regenerated doc"
- git push



