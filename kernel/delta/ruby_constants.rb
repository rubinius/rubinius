# Ruby constants
#

RUBY_ENGINE       = "rbx"
RUBY_PLATFORM     = Rubinius::HOST.dup
RUBY_RELEASE_DATE = Rubinius::RELEASE_DATE.dup
RUBY_COPYRIGHT    = "rubinius - Copyright (C) 2006-2011 Evan Phoenix"
RUBY_REVISION     = 0
# Must be last, it reads the above constants
RUBY_DESCRIPTION  = Rubinius.version

# Deprecated Ruby constants
#
VERSION           = RUBY_VERSION
PLATFORM          = RUBY_PLATFORM
