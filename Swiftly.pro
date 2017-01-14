TEMPLATE = subdirs

SUBDIRS = qt-mustache \
          Swiftly \
          Examples

Swiftly.depends = qt-mustache
Examples.depends = Swiftly
