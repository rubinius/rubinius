#---
# Excerpted from "Agile Web Development with Rails, 3rd Ed.",
# published by The Pragmatic Bookshelf.
# Copyrights apply to this code. It may not be used to create training material, 
# courses, books, articles, and the like. Contact us if you are in doubt.
# We make no guarantees that this code is fit for any purpose. 
# Visit http://www.pragmaticprogrammer.com/titles/rails3 for more book information.
#---
require File.dirname(__FILE__) + '/../test_helper'

class ProductsControllerTest < ActionController::TestCase
  def test_should_get_index
    get :index
    assert_response :success
    assert_not_nil assigns(:products)
  end

  def test_should_get_new
    get :new
    assert_response :success
  end

  def test_should_create_product
    assert_difference('Product.count') do
      post(:create,
           :product => {
             :title => "Rails Sucks",
             :description => "No really... their code is horrible",
             :price => 666,
             :image_url => "sux0rs.png",
           })
    end

    assert_redirected_to product_path(assigns(:product))
  end

  def test_should_show_product
    get :show, :id => products(:ruby_book).id
    assert_response :success
  end

  def test_should_get_edit
    get :edit, :id => products(:ruby_book).id
    assert_response :success
  end

  def test_should_update_product
    put :update, :id => products(:ruby_book).id, :product => { }
    assert_redirected_to product_path(assigns(:product))
  end

  def test_should_destroy_product
    assert_difference('Product.count', -1) do
      delete :destroy, :id => products(:ruby_book).id
    end

    assert_redirected_to products_path
  end
end
