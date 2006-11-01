require 'parse_tree'
require 'sydney_parser'

class SydneyParser
    
    # Generate a sexp which includes comments from +var+. +var+ is either a String
    # or something that can become an IO object.
    def self.unified_sexp(var)
        if String === var
            syd = load_string var, true
        else
            syd = load_file var.to_io, false, true
        end
        
        return syd.unified_sexp
        
    end
    
    # Generate a sexp which includes comments from +var+.
    def unified_sexp
        comms = collapse_per_line comments()
        x = sexp(true)
        insert_comments(x, comms)
    end
    
    def insert_comments(x)
      comms = collapse_per_line comments()
      insert_comments(x, comms)      
    end
    
    private
    
    # Turn an array of comments into a hash on the line.
    def collapse_per_line(comments)
        out = Hash.new
        comments.each do |line, column, text|
            if ary = out[line]
                ary << text
            else
                out[line] = [text]
            end
        end
        out
    end
    
    # Give +line+ find all the comments for it from +lines+ and +comments+.
    # Note that this modifies +lines+ so future calls doen't return already
    # used comments.
    def find_comments(lines, comments, line)
        found = []
        lines.each do |l|
            if l <= line
                found << l
            end
        end
        
        found_comments = found.sort.map do |l|
            lines.delete(l)
            comments[l] 
        end
        
        # puts "Found #{found_comments.flatten.inspect} #{found.inspect} comments for line #{line}"
        found_comments.flatten
    end
    
    # Recursively decend through sexp and insert :comment nodes from +comments+
    def insert_comments(sexp, comments)
        lines = comments.keys.sort
        out = insert_comments_rec(sexp, lines, comments)
        out[1]
    end
    
    # Recursive method that injects comments from +comments+ into +sexp+.
    def insert_comments_rec(sexp, lines, comments)
        # Dont let block or scope process comments so that we get the comment
        # as close to the actual element the programmer typed as possible.
        tag = sexp.first
        if tag == :block
            init_comments = nil
        elsif tag == :scope
            # Scope has a line number on it, but we dont want it there.
            sexp.pop
            init_comments = nil
        else
            line = sexp.last
            if Fixnum === line
                sexp.pop
                init_comments = find_comments(lines, comments, line)
                if init_comments.size > 0
                    init_comments.unshift :comment
                else
                    init_comments = nil
                end
            else
                init_comments = nil
            end
        end
        
        out = []
        sexp.each do |x|
             if Array === x
                 outcom, sub = insert_comments_rec(x, lines, comments)
                 out << outcom if outcom
                 out << sub
             else
                 out << x
             end
        end
        
        [init_comments, out]
    end
    
    # NOTE the semantics of these methods are critical. Changing them
    # can cause segfaults in the parser, so be careful!
    
    # Initialize the local variable information.
    def local_init
        @current_table = []
        @locals = [@current_table]
        @current_dvars = nil
        @dvars = []
    end
    
    # The 'special' globals that ruby stores in the local sope
    BaseLocals = [:_, :~]
    
    LocalsOffset = 2
    
    @@testing = false
    
    # Called when we are all done processing locals. (currently a noop)
    def local_finish
        
    end
    
    # Indicates if +var+ is defined in the current locals table.
    def local_defined?(var)
        puts "Checking for local :#{var}" if @@testing
        return @current_table.include?(var)
    end
    
    # Retrieve the index of +var+ in the locals table. If +var+ doesn't
    # exist, it adds it to the locals table.
    def local_cnt(var)
        puts "Getting index of local :#{var}" if @@testing
        if idx = BaseLocals.index(var)
            return idx
        end
        
        if idx = @current_table.index(var)
            return idx + LocalsOffset
        else
            local_append(var)
        end
    end
    
    # Add +var+ to the current locals table.
    def local_append(var)
        puts "Adding index for :#{var}" if @@testing
        idx = @current_table.size
        @current_table << var
        return idx + LocalsOffset
    end
    
    # Push the current locals table and get a new one.
    def local_push(top)
        puts "Pushing new locals #{@locals.inspect}" if @@testing
        @current_table = []
        @locals << @current_table
    end
    
    # Retrieve the current table of local variables.
    def local_table
        puts "Return local table. #{@current_table.inspect}" if @@testing
        @current_table
    end
    
    # Make the current locals table table the last one used.
    def local_pop
        puts "Popping locals #{@locals.inspect}" if @@testing
        return unless @locals
        @locals.pop
        @current_table = @locals.last
    end
    
    # Indicates if it's possible for there to be dynamic (block) variables
    def dyna_in_block
        ans = @dvars.size > 0
        # puts "in_block? #{ans.inspect}"
        return ans
    end
    
    # Allocate a new empty set of dynamic (block) variables, saving the old
    # stack of dynamic variables and returns it.
    def dyna_push
        # puts "Pushing new dvars"
        vars = Hash.new
        @dvars << vars
        @current_dvars = vars
        return @dvars
    end
    
    # Make the current dynamic variable table the last one used from +var+.
    def dyna_pop(vars)
        # puts "popping dvars"
        vars.pop
        @dvars = vars
        @current_dvars = @dvars.last
    end
    
    # Returns the current table of dynamic variables.
    def dyna_vars
        # puts "returning dvars #{@current_dvars.keys.inspect}"
        @current_dvars.keys
    end
    
    # Retrieve the current table of unique dynamic variables that are not in +pre+.
    def dyna_init(pre)
        # puts "doing init"
        # p @current_dvars
        # p pre
        return @current_dvars.keys - pre
    end
    
    # Indicates if +var+ is defined as a dynamic variable in any table of stack.
    def dvar_defined(var)
        @dvars.each do |vars|
            ans = vars.key? var
            return true if ans
        end
        return false
    end
    
    # Indicates if +var+ is a dynamic variable in only the current table.
    def dvar_curr(var)
        return false unless @current_dvars
        
        ans = @current_dvars.key? var
        # puts "checking for dvar #{var} #{ans.inspect}"
        return ans
    end
    
    # Set the dynamic variable +sym+ the value +val+ in the current table.
    def dvar_push(sym, val)
        # puts "Adding dvar #{sym}"
        @current_dvars[sym] = val
    end
end
