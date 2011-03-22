#
#   irb/lc/help-message.rb - 
#   	$Release Version: 0.9.5$
#   	$Revision: 11708 $
#   	$Date: 2007-02-12 15:01:19 -0800 (Mon, 12 Feb 2007) $
#   	by Keiju ISHITSUKA(keiju@ruby-lang.org)
#
# --
#
#   
#
<<HELP
Usage:  irb.rb [options] [programfile] [arguments]
  -f		    Suppress read of ~/.irbrc 
  -m		    Bc mode (load mathn, fraction or matrix are available)
  -d                Set $DEBUG to true (same as `ruby -d')
  -r load-module    Same as `ruby -r'
  -I path           Specify $LOAD_PATH directory
  --inspect	    Use `inspect' for output (default except for bc mode)
  --noinspect	    Don't use inspect for output
  --readline	    Use Readline extension module
  --noreadline	    Don't use Readline extension module
  --prompt prompt-mode
  --prompt-mode prompt-mode
		    Switch prompt mode. Pre-defined prompt modes are
		    `default', `simple', `xmp' and `inf-ruby'
  --inf-ruby-mode   Use prompt appropriate for inf-ruby-mode on emacs. 
		    Suppresses --readline. 
  --simple-prompt   Simple prompt mode
  --noprompt	    No prompt mode
  --tracer	    Display trace for each execution of commands.
  --back-trace-limit n
		    Display backtrace top n and tail n. The default
		    value is 16. 
  --irb_debug n	    Set internal debug level to n (not for popular use)
  -v, --version	    Print the version of irb
HELP
