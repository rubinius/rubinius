$: << File.expand_path("~/Work/p4/zss/src/ParseTree/dev/lib") # for me, not you.
require 'sexp'
require 'ruby_parser_extras'

class RubyLexer
  attr_accessor :command_start
  attr_accessor :cmdarg
  attr_accessor :cond
  attr_accessor :nest

  # Additional context surrounding tokens that both the lexer and
  # grammar use.
  attr_reader :lex_state

  attr_accessor :lex_strterm

  attr_accessor :parser # HACK for very end of lexer... *sigh*

  # Stream of data that yylex examines.
  attr_reader :src

  # Last token read via yylex.
  attr_accessor :token

  # Tempory buffer to build up a potential token.  Consumer takes
  # responsibility to reset this before use.
  attr_accessor :token_buffer

  # Value of last token which had a value associated with it.
  attr_accessor :yacc_value

  # What handles warnings
  attr_accessor :warnings

  EOF = :eof_haha!

  # ruby constants for strings (should this be moved somewhere else?)
  STR_FUNC_BORING = 0x00
  STR_FUNC_ESCAPE = 0x01 # TODO: remove and replace with REGEXP
  STR_FUNC_EXPAND = 0x02
  STR_FUNC_REGEXP = 0x04
  STR_FUNC_AWORDS = 0x08
  STR_FUNC_SYMBOL = 0x10
  STR_FUNC_INDENT = 0x20 # <<-HEREDOC

  STR_SQUOTE = STR_FUNC_BORING
  STR_DQUOTE = STR_FUNC_BORING | STR_FUNC_EXPAND
  STR_XQUOTE = STR_FUNC_BORING | STR_FUNC_EXPAND
  STR_REGEXP = STR_FUNC_REGEXP | STR_FUNC_ESCAPE | STR_FUNC_EXPAND
  STR_SSYM   = STR_FUNC_SYMBOL
  STR_DSYM   = STR_FUNC_SYMBOL | STR_FUNC_EXPAND

  # How the parser advances to the next token.
  #
  # @return true if not at end of file (EOF).

#   if ENV['SPY'] then
#     @@stats = Hash.new 0
#
#     def self.stats
#       @@stats
#     end
#
#     at_exit {
#       require 'pp'
#       pp RubyLexer.stats.sort_by {|k,v| -v}.first(20)
#     }
#   end

  def advance
    r = yylex
    self.token = r

    @@stats[r] += 1 if ENV['SPY']

    return r != RubyLexer::EOF
  end

  def arg_ambiguous
    self.warning("Ambiguous first argument. make sure.")
  end

  def comments
    c = @comments.join
    @comments.clear
    c
  end

  def expr_beg_push val
    cond.push false
    cmdarg.push false
    self.lex_state = :expr_beg
    self.yacc_value = s(val)
  end

  def fix_arg_lex_state
    self.lex_state = if lex_state == :expr_fname || lex_state == :expr_dot
                       :expr_arg
                     else
                       :expr_beg
                     end
  end

  def heredoc here # Region has 63 lines, 1595 characters
    _, eos, func, last_line = here

    indent  = (func & STR_FUNC_INDENT) != 0
    expand  = (func & STR_FUNC_EXPAND) != 0
    eos_re  = indent ? /[ \t]*#{eos}(\r?\n|\z)/ : /#{eos}(\r?\n|\z)/
    err_msg = "can't match #{eos_re.inspect} anywhere in "

    rb_compile_error err_msg if
      src.eos?

    if src.beginning_of_line? && src.scan(eos_re) then
      src.unread_many last_line # TODO: figure out how to remove this
      self.yacc_value = s(eos)
      return :tSTRING_END
    end

    token_buffer.clear

    if expand then
      case
      when src.scan(/#[$@]/) then
        src.pos -= 1 # FIX omg stupid
        self.yacc_value = s(src.matched)
        return :tSTRING_DVAR
      when src.scan(/#[{]/) then
        self.yacc_value = s(src.matched)
        return :tSTRING_DBEG
      when src.scan(/#/) then
        token_buffer << '#'
      end

      until src.scan(eos_re) do
        c = tokadd_string func, "\n", nil

        rb_compile_error err_msg if
          c == RubyLexer::EOF

        if c != "\n" then
          self.yacc_value = s(:str, token_buffer.join.delete("\r"))
          return :tSTRING_CONTENT
        else
          token_buffer << src.scan(/\n/)
        end

        rb_compile_error err_msg if
          src.eos?
      end

      # tack on a NL after the heredoc token - FIX NL should not be needed
      src.unread_many(eos + "\n") # TODO: remove this... stupid stupid stupid
    else
      until src.check(eos_re) do
        token_buffer << src.scan(/.*(\n|\z)/)
        rb_compile_error err_msg if
          src.eos?
      end
    end

    self.lex_strterm = s(:heredoc, eos, func, last_line)
    self.yacc_value = s(:str, token_buffer.join.delete("\r"))

    return :tSTRING_CONTENT
  end

  def heredoc_identifier
    term, func = nil, STR_FUNC_BORING
    token_buffer.clear

    case
    when src.scan(/(-?)(['"`])(.*?)\2/) then
      term = src[2]
      unless src[1].empty? then
        func |= STR_FUNC_INDENT
      end
      func |= case term
              when "\'" then
                STR_SQUOTE
              when '"' then
                STR_DQUOTE
              else
                STR_XQUOTE
              end
      token_buffer << src[3]
    when src.scan(/-?(['"`])(?!\1*\Z)/) then
      rb_compile_error "unterminated here document identifier"
    when src.scan(/(-?)(\w+)/) then
      term = '"'
      func |= STR_DQUOTE
      unless src[1].empty? then
        func |= STR_FUNC_INDENT
      end
      token_buffer << src[2]
    else
      return nil
    end

    if src.check(/.*\n/) then
      # TODO: think about storing off the char range instead
      line = src.string[src.pos, src.matched_size]
      src.string[src.pos, src.matched_size] = ''
    else
      line = nil
    end

    self.lex_strterm = s(:heredoc, token_buffer.join, func, line)

    if term == '`' then
      self.yacc_value = s("`")
      return :tXSTRING_BEG
    else
      self.yacc_value = s("\"")
      return :tSTRING_BEG
    end
  end

  def initialize
    self.token_buffer = []
    self.cond = StackState.new(:cond)
    self.cmdarg = StackState.new(:cmdarg)
    self.nest = 0
    @comments = []

    reset
  end

  def int_with_base base
    if src.matched =~ /__/ then
      rb_compile_error "Invalid numeric format"
    end
    self.yacc_value = src.matched.to_i(base)
    return :tINTEGER
  end

  def lex_state= o
    raise "wtf?" unless Symbol === o
    @lex_state = o
  end

  ##
  #  Parse a number from the input stream.
  #
  # @param c The first character of the number.
  # @return A int constant wich represents a token.

  def parse_number
    self.lex_state = :expr_end

    case
    when src.scan(/[+-]?0[xbd]\b/) then
      rb_compile_error "Invalid numeric format"
    when src.scan(/[+-]?0x[a-f0-9_]+/i) then
      return int_with_base(16)
    when src.scan(/[+-]?0b[01_]+/) then
      return int_with_base(2)
    when src.scan(/[+-]?0d[0-9_]+/) then
      return int_with_base(10)
    when src.scan(/[+-]?0o?[0-7_]*[89]/) then
      rb_compile_error "Illegal octal digit."
    when src.scan(/[+-]?0o?[0-7_]+|0o/) then
      return int_with_base(8)
    when src.scan(/[+-]?[\d_]+_(e|\.)/) then
      rb_compile_error "Trailing '_' in number."
    when src.scan(/[+-]?[\d_]+\.[\d_]+(e[+-]?[\d_]+)?\b|[+-]?[\d_]+e[+-]?[\d_]+\b/i) then
      number = src.matched
      if number =~ /__/ then
        rb_compile_error "Invalid numeric format"
      end
      self.yacc_value = number.to_f
      return :tFLOAT
    when src.scan(/[+-]?0\b/) then
      return int_with_base(10)
    when src.scan(/[+-]?[\d_]+\b/) then
      return int_with_base(10)
    else
      rb_compile_error "Bad number format"
    end
  end

  def parse_quote
    beg, nnd, short_hand, c = nil, nil, false, nil

    if src.scan(/[a-z0-9]{1,2}/i) then # Long-hand (e.g. %Q{}).
      rb_compile_error "unknown type of %string" if
        src.matched_size == 2
      c = src.matched
      beg = src.getch
      short_hand = false
      # HACK: stupid rubinius
      # c, beg, short_hand = src.matched, src.getch, false
    else                               # Short-hand (e.g. %{, %., %!, etc)
      c = 'Q'
      beg = src.getch
      short_hand = true
      # HACK: stupid rubinius
      # c, beg, short_hand = 'Q', src.getch, true
    end

    if src.eos? or c == RubyLexer::EOF or beg == RubyLexer::EOF then
      rb_compile_error "unterminated quoted string meets end of file"
    end

    # Figure nnd-char.  "\0" is special to indicate beg=nnd and that no nesting?
    nnd = { "(" => ")", "[" => "]", "{" => "}", "<" => ">" }[beg]
    nnd, beg = beg, "\0" if nnd.nil?

    token_type, self.yacc_value = nil, s("%#{c}#{beg}")
    token_type, string_type = case c
                              when 'Q' then
                                ch = short_hand ? nnd : c + beg
                                self.yacc_value = s("%#{ch}")
                                [:tSTRING_BEG,   STR_DQUOTE]
                              when 'q' then
                                [:tSTRING_BEG,   STR_SQUOTE]
                              when 'W' then
                                src.scan(/\s*/)
                                [:tWORDS_BEG,    STR_DQUOTE | STR_FUNC_AWORDS]
                              when 'w' then
                                src.scan(/\s*/)
                                [:tAWORDS_BEG,   STR_SQUOTE | STR_FUNC_AWORDS]
                              when 'x' then
                                [:tXSTRING_BEG,  STR_XQUOTE]
                              when 'r' then
                                [:tREGEXP_BEG,   STR_REGEXP]
                              when 's' then
                                self.lex_state  = :expr_fname
                                [:tSYMBEG,       STR_SSYM]
                              end

    rb_compile_error "Bad %string type. Expected [Qqwxr\W], found '#{c}'." if
      token_type.nil?

    self.lex_strterm = s(:strterm, string_type, nnd, beg)

    return token_type
  end

  def parse_string(quote)
    _, string_type, term, open = quote

    space = false # FIX: remove these
    func = string_type
    paren = open
    term_re = Regexp.escape term

    awords = (func & STR_FUNC_AWORDS) != 0
    regexp = (func & STR_FUNC_REGEXP) != 0
    expand = (func & STR_FUNC_EXPAND) != 0

    unless func then
      return :tSTRING_END
    end

    space = true if awords and src.scan(/\s+/)

    if self.nest == 0 && src.scan(/#{term_re}/) then
      if awords then
        quote[1] = nil
        return ' '
      elsif regexp then
        self.yacc_value = self.regx_options
        return :tREGEXP_END
      else
        self.yacc_value = s(term)
        return :tSTRING_END
      end
    end

    if space then
      return ' '
    end

    self.token_buffer.clear

    if expand
      case
      when src.scan(/#(?=[$@])/) then
        return :tSTRING_DVAR
      when src.scan(/#[{]/) then
        return :tSTRING_DBEG
      when src.scan(/#/) then
        token_buffer << '#'
      end
    end

    if tokadd_string(func, term, paren) == RubyLexer::EOF then
      rb_compile_error "unterminated string meets end of file"
    end

    self.yacc_value = s(:str, token_buffer.join)
    return :tSTRING_CONTENT
  end

  def rb_compile_error msg
    msg += ". near line #{src.lineno}: #{src.rest[/^.*/].inspect}"
    raise SyntaxError, msg
  end

  def read_escape
    case
    when src.scan(/\\/) then                   # Backslash
      '\\'
    when src.scan(/n/) then                    # newline
      "\n"
    when src.scan(/t/) then                    # horizontal tab
      "\t"
    when src.scan(/r/) then                    # carriage-return
      "\r"
    when src.scan(/f/) then                    # form-feed
      "\f"
    when src.scan(/v/) then                    # vertical tab
      "\13"
    when src.scan(/a/) then                    # alarm(bell)
      "\007"
    when src.scan(/e/) then                    # escape
      "\033"
    when src.scan(/b/) then                    # backspace
      "\010"
    when src.scan(/s/) then                    # space
      " "
    when src.scan(/[0-7]{1,3}/) then           # octal constant
      src.matched.to_i(8).chr
    when src.scan(/x([0-9a-fA-Fa-f]{2})/) then # hex constant
      src[1].to_i(16).chr
    when src.scan(/M-\\/) then
      c = self.read_escape
      c[0] = (c[0].ord | 0x80).chr
      c
    when src.scan(/M-(.)/) then
      c = src[1]
      c[0] = (c[0].ord | 0x80).chr
      c
    when src.scan(/C-\\|c\\/) then
      c = self.read_escape
      c[0] = (c[0].ord & 0x9f).chr
      c
    when src.scan(/C-\?|c\?/) then
      0177.chr
    when src.scan(/(C-|c)(.)/) then
      c = src[2]
      c[0] = (c[0].ord & 0x9f).chr
      c
    when src.scan(/[McCx0-9]/) || src.eos? then
      rb_compile_error("Invalid escape character syntax")
    else
      src.getch
    end
  end

  def regx_options
    good, bad = [], []

    if src.scan(/[a-z]+/) then
      good, bad = src.matched.split(//).partition { |s| s =~ /^[ixmonesu]$/ }
    end

    unless bad.empty? then
      rb_compile_error("unknown regexp option%s - %s" %
                       [(bad.size > 1 ? "s" : ""), bad.join.inspect])
    end

    return good.join
  end

  def reset
    self.command_start = true
    self.lex_strterm   = nil
    self.token         = nil
    self.yacc_value    = nil

    @src       = nil
    @lex_state = nil
  end

  def s(*args)
    result = Sexp.new(*args)
    result.line = src.lineno
    result.file = self.parser.file
    result
  end

  def src= src
    raise "bad src: #{src.inspect}" unless String === src
    @src = StringScanner.new(src) # HACK $src
  end

  def store_comment
    @comments.push(*self.token_buffer)
    self.token_buffer.clear
  end

  def tokadd_escape term
    case
    when src.scan(/\\\n/) then
      # just ignore
    when src.scan(/\\([0-7]{1,3}|x[0-9a-fA-F]{1,2})/) then
      self.token_buffer << src.matched
    when src.scan(/\\([MC]-|c)(?=\\)/) then
      self.token_buffer << src.matched
      self.tokadd_escape term
    when src.scan(/\\([MC]-|c)(.)/) then
      self.token_buffer << src.matched
    when src.scan(/\\[McCx]/) then
      rb_compile_error "Invalid escape character syntax"
    when src.scan(/\\(.)/m) then
      self.token_buffer << src.matched
    else
      rb_compile_error "Invalid escape character syntax"
    end
  end

  def tokadd_string(func, term, paren)
    awords = (func & STR_FUNC_AWORDS) != 0
    escape = (func & STR_FUNC_ESCAPE) != 0
    expand = (func & STR_FUNC_EXPAND) != 0
    regexp = (func & STR_FUNC_REGEXP) != 0
    symbol = (func & STR_FUNC_SYMBOL) != 0

    paren_re = paren.nil? ? nil : Regexp.new(Regexp.escape(paren))
    term_re  = Regexp.new(Regexp.escape(term))

    until src.eos? do
      c = nil
      case
      when paren_re && src.scan(paren_re) then
        self.nest += 1
      when self.nest == 0 && src.scan(term_re) then
        src.pos -= 1
        break
      when src.scan(term_re) then
        self.nest -= 1
      when ((awords && src.scan(/\s/)) ||
            (expand && src.scan(/#(?=[\$\@\{])/))) then
        src.pos -= 1
        break
      when awords && src.scan(/\\\n/) then
        token_buffer << "\n"
        next
      when expand && src.scan(/\\\n/) then
        next
      when awords && src.scan(/\\\s/) then
        c = ' '
      when (expand && src.scan(/#(?!\n)/)) || src.scan(/\\\n/) then
        # do nothing
      when src.scan(/\\\\/) then
        if escape then
          token_buffer << '\\'
        end
        c = '\\'
      when regexp && src.check(/\\/) then
        self.tokadd_escape term
        next
      when expand && src.scan(/\\/) then
        c = self.read_escape
      when src.scan(/\\/) then
        unless src.scan(term_re) || paren.nil? || src.scan(paren_re) then
          token_buffer << "\\"
        end
        # \\ case:
        # else if (ismbchar(c)) {
        #   int i, len = mbclen(c)-1;
        #   for (i = 0; i < len; i++) {
        #     tokadd(c);
        #     c = nextc();
        #   }
        # }
      else
        c = src.getch # FIX: I don't like this style
        if symbol && src.scan(/\0/) then
          rb_compile_error "symbol cannot contain '\\0'"
        end
      end

      c = src.matched unless c
      token_buffer << c
    end # until

    c = src.matched unless c
    c = RubyLexer::EOF if src.eos?

    return c
  end

  def warning s
    # do nothing for now
  end

  ##
  # Returns the next token. Also sets yy_val is needed.
  #
  # @return Description of the Returned Value
  # TODO: remove ALL sexps coming from here and move up to grammar
  # TODO: only literal values should come up from the lexer.

  def yylex
    c = ''
    space_seen = false
    command_state = false
    token_buffer.clear
    src = self.src

    self.token = nil
    self.yacc_value = nil

    if lex_strterm then
      token = nil

      if lex_strterm[0] == :heredoc then
        token = self.heredoc(lex_strterm)
        if token == :tSTRING_END then
          self.lex_strterm = nil
          self.lex_state = :expr_end
        end
      else
        token = self.parse_string(lex_strterm)

        if token == :tSTRING_END || token == :tREGEXP_END then
          self.lex_strterm = nil
          self.lex_state = :expr_end
        end
      end

      return token
    end

    command_state = self.command_start
    self.command_start = false

    last_state = lex_state

    loop do
      case
      when src.scan(/\004|\032|\000/), src.eos? then # ^D, ^Z, EOF
        return RubyLexer::EOF
      when src.scan(/\ |\t|\f|\r|\13/) then # white spaces, 13 = '\v
        space_seen = true
        next
      when src.scan(/#|\n/) then
        c = src.matched
        if c == '#' then
          src.unread c # ok

          while src.scan(/\s*#.*(\n+|\z)/) do
            token_buffer << src.matched.gsub(/^ +#/, '#').gsub(/^ +$/, '')
          end

          self.store_comment

          if src.eos? then
            return RubyLexer::EOF
          end
        end
        # Replace a string of newlines with a single one

        src.scan(/\n+/)

        if [:expr_beg, :expr_fname,
            :expr_dot, :expr_class].include? lex_state then
          next
        end

        self.command_start = true
        self.lex_state = :expr_beg
        return "\n"
      when src.scan(/\*\*=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("**")
        return :tOP_ASGN
      when src.scan(/\*\*/) then
        self.yacc_value = s("**")
        self.fix_arg_lex_state
        return :tPOW
      when src.scan(/\*\=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("*")
        return :tOP_ASGN
      when src.scan(/\*/) then
        result = if lex_state.is_argument && space_seen && !src.check(/\s/) then
                   warning("`*' interpreted as argument prefix")
                   :tSTAR
                 elsif lex_state == :expr_beg || lex_state == :expr_mid then
                   :tSTAR
                 else
                   :tSTAR2
                 end

        self.yacc_value = s("*")

        self.fix_arg_lex_state

        return result
      when src.scan(/\!\=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("!=")
        return :tNEQ
      when src.scan(/\!~/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("!~")
        return :tNMATCH
      when src.scan(/\!/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("!")
        return :tBANG
      when src.scan(/\=\=\=/) then
        self.fix_arg_lex_state
        self.yacc_value = s("===")
        return :tEQQ
      when src.scan(/\=\=/) then
        self.fix_arg_lex_state
        self.yacc_value = s("==")
        return :tEQ
      when src.scan(/\=~/) then
        self.fix_arg_lex_state
        self.yacc_value = s("=~")
        return :tMATCH
      when src.scan(/\=>/) then
        self.fix_arg_lex_state
        self.yacc_value = s("=>")
        return :tASSOC
      when src.scan(/\=/) then
        # documentation nodes
        if src.was_begin_of_line and src.scan(/begin(?=\s)/) then
          self.token_buffer << '=' # FIX merge up
          self.token_buffer << src.matched

          unless src.scan(/.*?\n=end(\n|\z)/m) then
            rb_compile_error("embedded document meets end of file")
          end

          self.token_buffer << src.matched
          self.store_comment

          next
        else
          self.fix_arg_lex_state
          self.yacc_value = s("=")
          return '='
        end
      when src.scan(/\<\=\>/) then
        self.fix_arg_lex_state
        self.yacc_value = s("<=>")
        return :tCMP
      when src.scan(/\<\=/) then
        self.fix_arg_lex_state
        self.yacc_value = s("<=")
        return :tLEQ
      when src.scan(/\<\<\=/) then
        self.fix_arg_lex_state
        self.lex_state = :expr_beg
        self.yacc_value = s("\<\<")
        return :tOP_ASGN
      when src.scan(/\<\</) then
        if (! [:expr_end,    :expr_dot,
               :expr_endarg, :expr_class].include?(lex_state) &&
            (!lex_state.is_argument || space_seen)) then
          tok = self.heredoc_identifier
          if tok then
            return tok
          end
        end

        self.fix_arg_lex_state
        self.yacc_value = s("\<\<")
        return :tLSHFT
      when src.scan(/\</) then
        self.fix_arg_lex_state
        self.yacc_value = s("<")
        return :tLT
      when src.scan(/\>\=/) then
        self.fix_arg_lex_state
        self.yacc_value = s(">=")
        return :tGEQ
      when src.scan(/\>\>=/) then
        self.fix_arg_lex_state
        self.lex_state = :expr_beg
        self.yacc_value = s(">>")
        return :tOP_ASGN
      when src.scan(/\>\>/) then
        self.fix_arg_lex_state
        self.yacc_value = s(">>")
        return :tRSHFT
      when src.scan(/\>/) then
        self.fix_arg_lex_state
        self.yacc_value = s(">")
        return :tGT
      when src.scan(/\"/) then
        self.lex_strterm = s(:strterm, STR_DQUOTE, '"', "\0") # TODO: question this
        self.yacc_value = s("\"")
        return :tSTRING_BEG
      when src.scan(/\`/) then
        self.yacc_value = s("`")
        case lex_state
        when :expr_fname then
          self.lex_state = :expr_end
          return :tBACK_REF2
        when :expr_dot then
          self.lex_state = if command_state then
                             :expr_cmdarg
                           else
                             :expr_arg
                           end
          return :tBACK_REF2
        end
        self.lex_strterm = s(:strterm, STR_XQUOTE, '`', "\0")
        return :tXSTRING_BEG
      when src.scan(/\'/) then
        self.lex_strterm = s(:strterm, STR_SQUOTE, "\'", "\0")
        self.yacc_value = s("'")
        return :tSTRING_BEG
      when src.scan(/\?/) then
        if lex_state == :expr_end || lex_state == :expr_endarg then
          self.lex_state = :expr_beg
          self.yacc_value = s("?")
          return '?'
        end

        if src.eos? then
          rb_compile_error "incomplete character syntax"
        end

        if src.check(/\s|\v/) then
          unless lex_state.is_argument then
            c2 = case src.matched
                 when " " then
                   's'
                 when "\n" then
                   'n'
                 when "\t" then
                   't'
                 when "\v" then
                   'v'
                 when "\r" then
                   'r'
                 when "\f" then
                   'f'
                 end

            if c2 then
              warning("invalid character syntax; use ?\\" + c2)
            end
          end

          # ternary
          self.lex_state = :expr_beg
          self.yacc_value = s("?")
          return '?'
          # elsif ismbchar(c) then # ternary, also
          # rb_warn "multibyte character literal not supported yet; use ?\\#{c}"
          # support.unread c
          # self.lex_state = :expr_beg
          # return '?'
        elsif src.check(/\w(?=\w)/) then # ternary, also
          self.lex_state = :expr_beg
          self.yacc_value = s("?")
          return '?'
        end

        c = if src.scan(/\\/) then
              self.read_escape
            else
              src.getch
            end
        c[0] = (c[0].ord & 0xff).chr
        self.lex_state = :expr_end
        self.yacc_value = c[0].ord
        return :tINTEGER
      when src.scan(/\&&=/) then
        self.yacc_value = s("&&")
        self.lex_state = :expr_beg
        return :tOP_ASGN
      when src.scan(/\&&/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("&&")
        return :tANDOP
      when src.scan(/\&\=/) then
        self.yacc_value = s("&")
        self.lex_state = :expr_beg
        return :tOP_ASGN
      when src.scan(/&/) then
        result = if lex_state.is_argument && space_seen && !src.check(/\s/) then
                   warning("`&' interpreted as argument prefix")
                   :tAMPER
                 elsif lex_state == :expr_beg || lex_state == :expr_mid then
                   :tAMPER
                 else
                   :tAMPER2
                 end

        self.fix_arg_lex_state
        self.yacc_value = s("&")
        return result
      when src.scan(/\|\|\=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("||")
        return :tOP_ASGN
      when src.scan(/\|\|/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("||")
        return :tOROP
      when src.scan(/\|\=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("|")
        return :tOP_ASGN
      when src.scan(/\|/) then
        self.fix_arg_lex_state
        self.yacc_value = s("|")
        return :tPIPE
      when src.scan(/[+-]/) then
        sign = src.matched
        utype, type = if sign == "+" then
                        [:tUPLUS, :tPLUS]
                      else
                        [:tUMINUS, :tMINUS]
                      end

        if lex_state == :expr_fname || lex_state == :expr_dot then
          self.lex_state = :expr_arg
          if src.scan(/@/) then
            self.yacc_value = s("#{sign}@")
            return utype
          else
            self.yacc_value = s(sign)
            return type
          end
        end

        if src.scan(/\=/) then
          self.lex_state = :expr_beg
          self.yacc_value = s(sign)
          return :tOP_ASGN
        end

        if (lex_state == :expr_beg || lex_state == :expr_mid ||
            (lex_state.is_argument && space_seen && !src.check(/\s/))) then
          if lex_state.is_argument then
            arg_ambiguous
          end

          self.lex_state = :expr_beg
          self.yacc_value = s(sign)

          if src.check(/\d/) then
            if utype == :tUPLUS then
              return self.parse_number
            else
              return :tUMINUS_NUM
            end
          end

          return utype
        end

        self.lex_state = :expr_beg
        self.yacc_value = s(sign)
        return type
      when src.scan(/\.\.\./) then
        self.lex_state = :expr_beg
        self.yacc_value = s("...")
        return :tDOT3
      when src.scan(/\.\./) then
        self.lex_state = :expr_beg
        self.yacc_value = s("..")
        return :tDOT2
      when src.scan(/\.\d/) then
        rb_compile_error "no .<digit> floating literal anymore put 0 before dot"
      when src.scan(/\./) then
        self.lex_state = :expr_dot
        self.yacc_value = s(".")
        return :tDOT
      when src.check(/[0-9]/) then
        return parse_number
      when src.scan(/[\)\]\}]/) then
        cond.lexpop
        cmdarg.lexpop
        self.lex_state = :expr_end
        self.yacc_value = s(src.matched)
        result = {
          ")" => :tRPAREN,
          "]" => :tRBRACK,
          "}" => :tRCURLY
        }[src.matched]
        return result
      when src.scan(/::/) then
        if (lex_state == :expr_beg ||
            lex_state == :expr_mid ||
            lex_state == :expr_class ||
            (lex_state.is_argument && space_seen)) then
          self.lex_state = :expr_beg
          self.yacc_value = s("::")
          return :tCOLON3
        end

        self.lex_state = :expr_dot
        self.yacc_value = s("::")
        return :tCOLON2
      when src.scan(/\:/) then
        if (lex_state == :expr_end || lex_state == :expr_endarg ||
            src.check(/\s/)) then
          self.lex_state = :expr_beg
          self.yacc_value = s(":")
          return ':'
        end

        case
        when src.scan(/\'/) then
          self.lex_strterm = s(:strterm, STR_SSYM, src.matched, "\0")
        when src.scan(/\"/) then
          self.lex_strterm = s(:strterm, STR_DSYM, src.matched, "\0")
        end

        self.lex_state = :expr_fname
        self.yacc_value = s(":")
        return :tSYMBEG
      when src.scan(/\//) then
        if lex_state == :expr_beg || lex_state == :expr_mid then
          self.lex_strterm = s(:strterm, STR_REGEXP, '/', "\0")
          self.yacc_value = s("/")
          return :tREGEXP_BEG
        end

        if src.scan(/\=/) then
          self.yacc_value = s("/")
          self.lex_state = :expr_beg
          return :tOP_ASGN
        end

        if lex_state.is_argument && space_seen then
          unless src.scan(/\s/) then
            arg_ambiguous
            self.lex_strterm = s(:strterm, STR_REGEXP, '/', "\0")
            self.yacc_value = s("/")
            return :tREGEXP_BEG
          end
        end

        self.fix_arg_lex_state

        self.yacc_value = s("/")
        return :tDIVIDE
      when src.scan(/\^=/) then
        self.lex_state = :expr_beg
        self.yacc_value = s("^")
        return :tOP_ASGN
      when src.scan(/\^/) then
        self.fix_arg_lex_state
        self.yacc_value = s("^")
        return :tCARET
      when src.scan(/\;/) then
        self.command_start = true
        self.lex_state = :expr_beg
        self.yacc_value = s(";")
        return src.matched
      when src.scan(/\,/) then
        self.lex_state = :expr_beg
        self.yacc_value = s(",")
        return src.matched
      when src.scan(/\~/) then
        if lex_state == :expr_fname || lex_state == :expr_dot then
          src.scan(/@/)
        end

        self.fix_arg_lex_state
        self.yacc_value = s("~")

        return :tTILDE
      when src.scan(/\(/) then
        result = :tLPAREN2
        self.command_start = true
        if lex_state == :expr_beg || lex_state == :expr_mid then
          result = :tLPAREN
        elsif space_seen then
          if lex_state == :expr_cmdarg then
            result = :tLPAREN_ARG
          elsif lex_state == :expr_arg then
            warning("don't put space before argument parentheses")
            result = :tLPAREN2
          end
        end

        self.expr_beg_push "("

        return result
      when src.scan(/\[/) then
        result = src.matched

        if lex_state == :expr_fname || lex_state == :expr_dot then
          self.lex_state = :expr_arg
          case
          when src.scan(/\]\=/) then
            self.yacc_value = s("[]=")
            return :tASET
          when src.scan(/\]/) then
            self.yacc_value = s("[]")
            return :tAREF
          else
            rb_compile_error "unexpected '['"
          end
        elsif lex_state == :expr_beg || lex_state == :expr_mid then
          result = :tLBRACK
        elsif lex_state.is_argument && space_seen then
          result = :tLBRACK
        end

        self.expr_beg_push("[")

        return result
      when src.scan(/\{/) then
        result = if lex_state.is_argument || lex_state == :expr_end then
                   :tLCURLY      #  block (primary)
                 elsif lex_state == :expr_endarg then
                   :tLBRACE_ARG  #  block (expr)
                 else
                   :tLBRACE      #  hash
                 end

        self.expr_beg_push("{")

        return result
      when src.scan(/\\/) then
        if src.scan(/\n/) then
          space_seen = true
          next
        end
        rb_compile_error "bare backslash only allowed before newline"
      when src.scan(/\%/) then
        if lex_state == :expr_beg || lex_state == :expr_mid then
          return parse_quote
        end

        if src.scan(/\=/) then
          self.lex_state = :expr_beg
          self.yacc_value = s("%")
          return :tOP_ASGN
        end

        if lex_state.is_argument && space_seen && ! src.check(/\s/) then
          return parse_quote
        end

        self.fix_arg_lex_state
        self.yacc_value = s("%")

        return :tPERCENT
      when src.scan(/(\$_)(\w)/) then
        self.lex_state = :expr_end
        token_buffer << src[1]
        # HACK? c = src[2]
        # pass through
      when src.scan(/\$_/) then
        self.lex_state = :expr_end
        token_buffer << src.matched
        self.yacc_value = s(src.matched)
        return :tGVAR
      when src.scan(/\$[~*$?!@\/\\;,.=:<>\"]|\$-\w?/) then
        self.lex_state = :expr_end
        token_buffer << src.matched
        self.yacc_value = s(token_buffer.join)
        return :tGVAR
      when src.scan(/\$([\&\`\'\+])/) then
        self.lex_state = :expr_end
        # Explicit reference to these vars as symbols...
        if last_state == :expr_fname then
          token_buffer << src.matched
          self.yacc_value = s(token_buffer.join)
          return :tGVAR
        else
          self.yacc_value = s(:back_ref, src[1].to_sym)
          return :tBACK_REF
        end
      when src.scan(/\$[1-9]\d*/) then
        self.lex_state = :expr_end
        token_buffer.push(*src.matched.split(//))
        if last_state == :expr_fname then
          self.yacc_value = s(token_buffer.join)
          return :tGVAR
        else
          self.yacc_value = s(:nth_ref, token_buffer.join[1..-1].to_i)
          return :tNTH_REF
        end
      when src.scan(/\$0/) then
        self.lex_state = :expr_end
        token_buffer << '$' # why just this?!?
        # pass through
      when src.scan(/\$\W|\$\z/) then # TODO: remove?
        self.lex_state = :expr_end
        self.yacc_value = s("$")
        return '$'
      when src.scan(/\$/)
        self.lex_state = :expr_end
        token_buffer << src.matched
        src.getch
        # pass through
      when src.scan(/\@/) then
        token_buffer << '@'

        if src.scan(/(@)?\d/) then
          if src[1] then
            rb_compile_error "`@@#{c}` is not allowed as a class variable name"
          else
            rb_compile_error "`@#{c}' is not allowed as an instance variable name"
          end
        end

        if src.scan(/@/) then
          token_buffer << src.matched
        end

        unless src.scan(/\w/) then
          self.yacc_value = s("@")
          return '@'
        end
      when src.scan(/\_/) then
        if src.was_begin_of_line && src.scan(/_END__(\n|\Z)/) then
          return RubyLexer::EOF
        end
      else
        c = src.getch # FIX: I really hate this
        if c =~ /\W/ then
          rb_compile_error "Invalid char '#{c.inspect}' in expression"
        end
      end

      src.pos -= 1 # HACK
      if src.scan(/\w+/) then
        token_buffer.push(*src.matched.split(//)) # TODO: that split is tarded.
      end

      if token_buffer[0] =~ /\w/ && src.scan(/[\!\?](?!=)/) then
        token_buffer << src.matched
      end

      result = nil
      last_state = lex_state

      case token_buffer[0]
      when /^\$/ then
        self.lex_state = :expr_end
        result = :tGVAR
      when '@' then
        self.lex_state = :expr_end
        if token_buffer[1] == '@' then
          result = :tCVAR
        else
          result = :tIVAR
        end
      else
        if token_buffer[-1] =~ /[!?]/ then
          result = :tFID
        else
          if lex_state == :expr_fname then
            # ident=, not =~ => == or followed by =>
            if src.scan(/=(?:(?![~>=])|(?==>))/) then
              result = :tIDENTIFIER
              token_buffer << src.matched
            end
          end

          if result.nil? && token_buffer[0] =~ /[A-Z]/ then
            result = :tCONSTANT
          else
            result = :tIDENTIFIER
          end
        end

        unless lex_state == :expr_dot then
          # See if it is a reserved word.
          keyword = Keyword.keyword(token_buffer.join, token_buffer.length)

          unless keyword.nil? then
            state = lex_state
            self.lex_state = keyword.state

            self.yacc_value = if state == :expr_fname then
                                s(keyword.name)
                              else
                                s(token_buffer.join)
                              end

            if keyword.id0 == :kDO then
              self.command_start = true
              if cond.is_in_state then
                return :kDO_COND
              end
              if cmdarg.is_in_state && state != :expr_cmdarg then
                return :kDO_BLOCK
              end
              if state == :expr_endarg then
                return :kDO_BLOCK
              end
              return :kDO
            end

            if state == :expr_beg then
              return keyword.id0
            end

            if keyword.id0 != keyword.id1 then
              self.lex_state = :expr_beg
            end

            return keyword.id1
          end
        end # lex_state == :expr_dot

        if (lex_state == :expr_beg ||
            lex_state == :expr_mid ||
            lex_state == :expr_dot ||
            lex_state == :expr_arg ||
            lex_state == :expr_cmdarg) then
          if command_state then
            self.lex_state = :expr_cmdarg
          else
            self.lex_state = :expr_arg
          end
        else
          self.lex_state = :expr_end
        end
      end

      self.yacc_value = s(token_buffer.join)

      var = self.parser.env[self.yacc_value.to_sym]
      if var == :lvar && last_state != :expr_dot then # HACK: partial port
        # ((dyna_in_block()&&rb_dvar_defined(yylval.id))||local_id(yylval.id)))
        self.lex_state = :expr_end
      end

      return result
    end
  end
end
