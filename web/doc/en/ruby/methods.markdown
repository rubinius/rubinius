---
layout: doc_en
title: Methods
previous: Scripts
previous_url: ruby/scripts
next: Constants
next_url: ruby/constants
review: true
---

A ruby method is a wrapper of expressions that return a value. We have seen 
examples of methods in the previous chapter. To write a method in ruby you 
start by using the 'def' keyword, followed by the name of the method, the arguments 
and finally the 'end' keyword to indicate the end of the method.

"Ruby methods provide a way to organize code and promote re-use. 
Rather than create long sections of Ruby code, the code is instead organized 
into logical groups that can be called when needed and re-used without having to 
re-write the same code over and over. Methods are simple to use, in fact you only 
need to do two things with a method, declare it and call it." - techotopia

Example of ruby methods.

    def method_name
      # method body
      ...
    end

    def method_name(arg1, arg2)
      # method body
      ...
    end

The 'def' keyword indicates the beginning of a ruby method, the 'method_name' is the 
name of the method and the 'args' are the parameters that will be used inside the method 
while the 'method body' contains the sequence and logic of the method along with the 
return value.

basic_math.rb 

    ...

    def subtract(first_number, second_number)
      first_number.to_i - second_number.to_i
    end

    ...

This snippet from the basic_math.rb script shows a simple ruby method. The 'def' keyword 
indicates the start of a ruby method, the 'subtraction' is the name of the method, 
'first_number', 'second_number' are the parameters of the subtraction method and finally 
'end' keyword signifies the end of the method. The 'method body' returns the difference of 
subtracting the second_number from the first_number. To run the method you would do something like: 

    # This is a comment
    
    puts "#{subtract(10, 8)}" 
    
    # The value will be 2 because 10 - 8 = 2

The hash tag '#' serves two main purposes in ruby. One use is to insert a comment into your ruby code and the 
other is used in string interpolation. When you insert a comment into your ruby code as the example above shows, 
it is ignored by the ruby interpreter. In string interpolation, it is used to inject values into a string body. 
These values can be method calls, variables or values. The above example shows how we use string interpolation to 
call a method 'subtract' in a string. In ruby the return keyword is implicit by default, 
however it can be explicity declared inside the subtract method like this;

    def subtract(first_number, second_number)
      return first_number.to_i - second_number.to_i
    end

    puts "#{subtract(10, 8)}" 
    
    # The value will be 2

Everything in ruby is an object, therefore we tell the interpreter that we want to do a mathematical calculation 
thus we append the method "*.to_i" which means "parse this object as an integer" to the method arguments (first_number and second_number) 
so we can perform the mathematical subtraction. For further information refer to the documentation section of the ruby language site 
<a href="http://www.ruby-lang.org/en/documentation" target="_blank">here</a>.
