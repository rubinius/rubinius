require File.dirname(__FILE__) + '/../spec_helper'

# Specifying the behavior of operators in combination could
# lead to combinatorial explosion. A better way seems to be
# to use a technique from formal proofs that involve a set of
# equivalent statements. Suppose you have statements A, B, C.
# If they are claimed to be equivalent, this can be shown by
# proving that A implies B, B implies C, and C implies A.
# (Actually any closed circuit of implications.)
# 
# Here, we can use a similar technique where we show starting
# at the top that each level of operator has precedence over
# the level below (as well as showing associativity within
# the precedence level).

=begin
Excerpted from 'Programming Ruby: The Pragmatic Programmer's Guide'
Second Edition by Dave Thomas, Chad Fowler, and Andy Hunt, page 324

Table 22.4. Ruby operators (high to low precedence) 
Method   Operator                  Description 
-----------------------------------------------------------------------
         :: .                      
 x*      [ ] [ ]=                  Element reference, element set 
 x       **                        Exponentiation 
 x       ! ~ + -                   Not, complement, unary plus and minus 
                                   (method names for the last two are +@ and -@) 
 x       * / %                     Multiply, divide, and modulo 
 x       + -                       Plus and minus 
 x       >> <<                     Right and left shift 
 x       &                         “And” (bitwise for integers) 
 x       ^ |                       Exclusive “or” and regular “or” (bitwise for integers)
 x       <= < > >=                 Comparison operators 
 x       <=> == === != =~ !~       Equality and pattern match operators (!= 
                                   and !~ may not be defined as methods) 
         &&                        Logical “and” 
         ||                        Logical “or” 
         .. ...                    Range (inclusive and exclusive) 
         ? :                       Ternary if-then-else 
         = %= /= -= += |= &=       Assignment
         >>= <<= *= &&= ||= **=
         defined?                  Check if symbol defined 
         not                       Logical negation 
         or and                    Logical composition 
         if unless while until     Expression modifiers 
         begin/end                 Block expression
-----------------------------------------------------------------------

* Operators marked with 'x' in the Method column are implemented as methods
and can be overridden (except != and !~ as noted). (But see the specs 
below for implementations that define != and !~ as methods.)

** These are not included in the excerpted table but are shown here for
completeness.
=end

# TODO : Check the operations and precedence 
describe "Operators" do

end
