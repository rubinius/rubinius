#---
# Excerpted from "Agile Web Development with Rails, 3rd Ed.",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/rails3 for more book information.
#---

class AddTestData < ActiveRecord::Migration
  def self.up
    Product.delete_all

    Product.create(:title => 'Pragmatic Project Automation',
    :description => 
    %{<p>
       <em>Pragmatic Project Automation</em> shows you how to improve the 
       consistency and repeatability of your project's procedures using 
       automation to reduce risk and errors.
      </p>
      <p>
        Simply put, we're going to put this thing called a computer to work 
        for you doing the mundane (but important) project stuff. That means 
        you'll have more time and energy to do the really 
        exciting---and difficult---stuff, like writing quality code.
      </p>},
    :image_url =>   '/images/auto.jpg',    
    :price => 29.95)


    Product.create(:title => 'Pragmatic Version Control',
      :description =>
      %{<p>
         This book is a recipe-based approach to using Subversion that will 
         get you up and running quickly---and correctly. All projects need
         version control: it's a foundational piece of any project's 
         infrastructure. Yet half of all project teams in the U.S. don't use
         any version control at all. Many others don't use it well, and end 
         up experiencing time-consuming problems.
      </p>},
    :image_url => '/images/svn.jpg',
    :price => 28.50)
    # . . .


    Product.create(:title => 'Pragmatic Unit Testing (C#)',
    :description => 
    %{<p>
        Pragmatic programmers use feedback to drive their development and 
        personal processes. The most valuable feedback you can get while 
        coding comes from unit testing.
      </p>
      <p>
        Without good tests in place, coding can become a frustrating game of 
        "whack-a-mole." That's the carnival game where the player strikes at a 
        mechanical mole; it retreats and another mole pops up on the opposite side 
        of the field. The moles pop up and down so fast that you end up flailing 
        your mallet helplessly as the moles continue to pop up where you least 
        expect them.
      </p>},
    :image_url => '/images/utc.jpg',
    :price => 27.75)


  end

  def self.down
    Product.delete_all
  end
end

