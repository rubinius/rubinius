require 'rbyaml/emitter'
require 'rbyaml/serializer'
require 'rbyaml/representer'
require 'rbyaml/resolver'

module RbYAML
  #
  # Constants
  #
  VERSION = '0.20'
  SUPPORTED_YAML_VERSIONS = ['1.0','1.1']

  LINE_BR_REG = /[\n\x85]|(?:\r[^\n])/
  NON_PRINTABLE_RE = /[^\x09\x0A\x0D\x20-\x7E\x85\xA0-\xFF]/

  ENDING = /^---[\0 \t\r\n\x85]$/
  START = /^\.\.\.[\0 \t\r\n\x85]$/
  NULL_OR_OTHER = "\0 \t\r\n\x85"
  #    BEG = /^([^\0 \t\r\n\x85\-?:,\[\]{}#&*!|>'"%@`]|([\-?:][^\0 \t\r\n\x85]))/    #Since current SYCK handles this one wrong, we have to allow backtick right now.
  BEG = /^([^\0 \t\r\n\x85\-?:,\[\]{}#&*!|>'"%@]|([\-?:][^\0 \t\r\n\x85]))/    
  NULL_OR_LINEBR = "\0\r\n\x85"
  ALPHA_REG = /[-0-9A-Za-z_]/
  NULL_BL_LINEBR = "\0 \r\n\x85"
  NULL_BL_T_LINEBR = NULL_OR_OTHER
  NON_ALPHA = /[^-0-9A-Za-z_]/   
  NON_ALPHA_OR_NUM = "\0 \t\r\n\x85?:,]}%@`"
  NULL_T_BL_LINEBR = NULL_OR_OTHER
  BLANK_T = " \t"
  PLUS_MIN = /[+-]/
  BLANK_OR_LINEBR = " \r\n\x85"
  FULL_LINEBR = "\r\n\x85"
  UNESCAPES = {
    "0"  =>   "\0",
    "a"  =>   "\x07",
    "b"  =>   "\x08",
    "t"  =>   "\x09",
    "\t" =>   "\x09",
    "n"  =>   "\x0A",
    "v"  =>   "\x0B",
    "f"  =>   "\x0C",
    "r"  =>   "\x0D",
    "e"  =>   "\x1B",
    " "  =>   "\x20",
    '"'  =>   '"',
    "\\" =>   "\\",
    "N"  =>   "\x85",
    "_"  =>   "\xA0"
  }
  
  ESCAPE_CODES = {
    'x' =>   2
  }


  ESCAPE_REPLACEMENTS = {
    ?\0   =>   "0",
    ?\x07 =>   "a",
    ?\x08 =>   "b",
    ?\x09 =>   "t",
    ?\x0A =>   "n",
    ?\x0B =>   "v",
    ?\x0C =>   "f",
    ?\x0D =>   "r",
    ?\x1B =>   "e",
    ?"    =>   "\"",
    ?\\   =>   "\\",
    ?\x85 =>   "N",
    ?\xA0 =>   "_"
  }
  
  SPACES_AND_STUFF = "'\"\\\0 \t\r\n\x85"
  DOUBLE_ESC = "\"\\"
  NOT_HEXA = /[^0-9A-Fa-f]/
  R_flowzero = /[\0 \t\r\n\x85]|(:[\0 \t\r\n\x28])/
  R_flownonzero = /[\0 \t\r\n\x85\[\]{},:?]/
  S4 = "\0 \t\r\n\x28[]{}"
  END_OR_START = /^(---|\.\.\.)[\0 \t\r\n\x85]$/
  STRANGE_CHR = /[\]\[\-';\/?:@&=+$,.!~*()%\w]/
  HEXA_REG = /[0-9A-Fa-f]/
  
  #
  # Default settings
  #
  DEFAULTS = {
    :Indent => 2, :UseHeader => false, :UseVersion => false, :Version => '1.1',
    :ExplicitStart => true, :ExplicitEnd => false,
    :SortKeys => false, :AnchorFormat => 'id%03d', :ExplicitTypes => false,
    :Canonical => false,
    :WidthType => 'absolute', :BestWidth => 80,
    :UseBlock => false, :UseFold => false, :Encoding => :None,
    :UsePlain => false, :UseSingle => false, :UseDouble => false,
    :Emitter => RbYAML::Emitter, :Serializer => RbYAML::Serializer, 
    :Representer => RbYAML::Representer, :Resolver => RbYAML::Resolver
  }
end
